#pragma once

#include "Core.h"

#include "Allocator.h"
#include "Memory.h"
#include "Ranger.h"
#include <new>

#if (_DEBUG)
#include "Assert.h"
#endif

namespace GTSL
{
	template<typename T, class ALLOCATOR>
	class FlatHashMap
	{
	public:
		using key_type = uint64;

		FlatHashMap() = default;

		FlatHashMap(const uint32 size, const AllocatorReference& allocatorReference) : capacity(size), allocator(allocatorReference)
		{
			GTSL_ASSERT(size != 0 && (size & (size - 1)) == 0, "Size is not a power of two!");
			GTSL_ASSERT(static_cast<uint32>(size * this->loadFactor) != 0, "Size and load factor combination leads to empty buckets!")
			this->data = allocate(size, getMaxBucketLength()); build(0, getMaxBucketLength());
		}

		FlatHashMap(const uint32 size, const float32 loadFactor, const AllocatorReference& allocatorReference) : capacity(size), loadFactor(loadFactor), allocator(allocatorReference)
		{
			GTSL_ASSERT(size != 0 && (size & (size - 1)) == 0, "Size is not a power of two!");
			GTSL_ASSERT(this->loadFactor < 1.0f && this->loadFactor > 0.0f, "Invalid load factor!");
			GTSL_ASSERT(static_cast<uint32>(size * this->loadFactor) != 0, "Size and load factor combination leads to empty buckets!")
			this->data = allocate(size, getMaxBucketLength()); build(0, getMaxBucketLength());
		}

		FlatHashMap(FlatHashMap&& other) noexcept : data(other.data), capacity(other.capacity), loadFactor(other.loadFactor), allocator(GTSL::MakeTransferReference(other.allocator))
		{
			other.data = nullptr; other.capacity = 0; other.loadFactor = 0.0f;
		}

		~FlatHashMap() { if (this->data) { deallocate(getMaxBucketLength()); } }

		template<typename TT>
		class Iterator
		{
		public:
			Iterator(FlatHashMap* hashMap) : map(hashMap)
			{
			}

			Iterator(FlatHashMap* hashMap, uint32 vector, uint32 index) : map(hashMap), bucket(vector), index(index), bucketLength(map->getBucketLength(this->bucket))
			{
			}

			Iterator operator++()
			{
				auto range = map->getValuesBucket(bucket);

				if (index + 1 < bucketLength)
				{
					++index;
				}
				else //if bucket is over
				{
					for (uint32 i = bucket + 1; i < map->capacity; ++i) //search for next non empty bucket
					{
						if ((bucketLength = map->getBucketLength(i)) != 0) //vector with some element/s
						{
							bucket = i; index = 0; return (*this);
						}
					}

					++index;
				}

				return (*this);
			}

			Iterator operator--()
			{
				auto range = map->getValuesBucket(bucket);
				
				if (range.begin() + index == range.begin())
				{
					--bucket; index = 0;
				}
				else
				{
					--index;
				}

				return (*this);
			}

			Iterator operator++(int) { Iterator ret = *this; ++(*this); return ret; }
			Iterator operator--(int) { Iterator ret = *this; --(*this); return ret; }

			TT& operator*() const { return *(map->getValuesBucket(bucket) + index); }
			TT* operator->() const { return &(map->getValuesBucket(bucket) + index); }

			bool operator==(const Iterator& other) const { return bucket == other.bucket && index == other.index; }

			bool operator!=(const Iterator& other) const { return bucket != other.bucket || index != other.index; }

		private:
			FlatHashMap* map{ nullptr };
			uint32 bucket{ 0 }; uint32 index{ 0 }; uint32 bucketLength{ 0 };
		};

		Iterator<T> begin()
		{
			uint32 bucket{ 0 }, index{ 0 };
			for (uint32 i = 0; i < this->capacity; ++i) { if (this->getBucketLength(i, getMaxBucketLength()) != 0) { bucket = i; break; } }
			return Iterator<T>(this, bucket, index);
		}
		
		Iterator<T> end()
		{
			uint32 bucket{ 0 }, index{ 0 };
			for (uint32 i = this->capacity; i > 0; --i) { if ((index = this->getBucketLength(i - 1, getMaxBucketLength())) != 0) { bucket = i - 1; break; } }
			return Iterator<T>(this, bucket, index);
		}

		template<typename... ARGS>
		T& Emplace(const key_type key, ARGS&&... args)
		{
			const auto max_bucket_length = getMaxBucketLength();
			const auto bucket = modulo(key, this->capacity);
			GTSL_ASSERT(findKeyInBucket(bucket, key, max_bucket_length) == nullptr, "Key already exists!")
			uint64 place_index = getBucketLength(bucket, max_bucket_length)++;
			if (place_index + 1 > max_bucket_length) { resize(max_bucket_length); }
			getKeysBucket(bucket, max_bucket_length)[place_index] = key;
			return *(new(getValuesBucket(bucket, max_bucket_length) + place_index) T(MakeForwardReference<ARGS>(args)...));
		}

		bool Find(const key_type key) { return findKeyInBucket(modulo(key, this->capacity), key, getMaxBucketLength()); }
		
		bool Find(const key_type key, T*& obj)
		{
			const auto max_bucket_length = getMaxBucketLength(); const auto bucket = modulo(key, this->capacity);
			key_type* ret = findKeyInBucket(bucket, key, max_bucket_length);
			obj = (getValuesBucketPointer(bucket, max_bucket_length) + (ret - (getKeysBucketPointer(bucket, max_bucket_length) + 1)));
			return ret;
		}
		
		T& At(const key_type key)
		{
			const auto max_bucket_length = getMaxBucketLength();
			const auto bucket = modulo(key, capacity); const auto index = getIndexForKeyInBucket(bucket, key, max_bucket_length);
			GTSL_ASSERT(index != 0xFFFFFFFF, "No element with that key!");
			return getValuesBucket(bucket, max_bucket_length)[index];
		}

		void Remove(const key_type key)
		{
			const auto max_bucket_length = getMaxBucketLength();
			auto bucket = modulo(key, this->capacity); auto index = getIndexForKeyInBucket(bucket, key, max_bucket_length);
			key_type bucket_length = getBucketLength(bucket, max_bucket_length)--;
			GTSL_ASSERT(findKeyInBucket(bucket, key, max_bucket_length) == nullptr, "Key doesn't exist!")
			getKeysBucket(bucket, max_bucket_length)[index].~T();
			MemCopy((bucket_length - index) * sizeof(key_type), getKeysBucket(bucket, max_bucket_length) + index + 1, getKeysBucket(bucket, max_bucket_length) + index);
			MemCopy((bucket_length - index) * sizeof(T), getValuesBucket(bucket, max_bucket_length) + index + 1, getValuesBucket(bucket, max_bucket_length) + index);
		}
		
	private:
		friend class Iterator<T>;

		byte* data = nullptr;
		uint32 capacity = 0;
		float32 loadFactor = 0.5f;
		[[no_unique_address]] ALLOCATOR allocator;

		[[nodiscard]] key_type* findKeyInBucket(const uint32 bucket, const key_type key, const uint32 maxBucketLength) const
		{
			//const auto simd_elements = modulo(getBucketLength(bucket), this->capacity);
			//
			//SIMD128<key_type> key_vector(key);
			//
			//for (uint32 i = 0; i < simd_elements; ++i)
			//{
			//	SIMD128<key_type> keys(UnalignedPointer<uint64>(getKeysBucket(bucket)));
			//	auto res = keys == key_vector;
			//	auto bb = res.GetElement<0>();
			//	if(bb & key || cc & key)
			//}
			
			for (auto& e : getKeysBucket(bucket, maxBucketLength)) { if (e == key) { return &e; } } return nullptr;
		}

		[[nodiscard]] uint32 getIndexForKeyInBucket(const uint32 bucket, const key_type key, const uint32 maxBucketLength) const
		{
			for (auto& e : getKeysBucket(bucket, maxBucketLength)) { if (e == key) { return static_cast<uint32>(&e - getKeysBucket(bucket, maxBucketLength).begin()); } } return 0xFFFFFFFF;
		}

		[[nodiscard]] Ranger<key_type> getKeysBucket(const uint32 bucket, const uint32 maxBucketLength) const { return Ranger<key_type>(getBucketLength(bucket, maxBucketLength), getKeysBucketPointer(bucket, maxBucketLength) + 1); }

		[[nodiscard]] Ranger<T> getValuesBucket(const uint32 bucket, const uint32 maxBucketLength) const { return Ranger<T>(getBucketLength(bucket, maxBucketLength), getValuesBucketPointer(bucket, maxBucketLength)); }

		static constexpr uint32 modulo(const key_type key, const uint32 size) { return key & (size - 1); }

		void resize(const uint32 maxBucketLength)
		{
			auto new_capacity = this->capacity * 2;
			auto new_alloc = allocate(new_capacity, maxBucketLength);
			copy(new_capacity, new_alloc, maxBucketLength);
			deallocate(maxBucketLength);
			this->data = new_alloc; this->capacity = new_capacity;
			build(this->capacity / 2, maxBucketLength);

			for(uint32 bucket = 0; bucket < this->capacity / 2; ++bucket)
			{
				auto element_index = 0;
				
				for(auto e : getKeysBucket(bucket, maxBucketLength))
				{
					auto new_bucket = modulo(e, this->capacity);
					
					if(new_bucket != bucket) //If after resizing, element would not still be in the same bucket, move it
					{	
						auto& current_bucket_length = getBucketLength(bucket, maxBucketLength); auto& new_bucket_length = getBucketLength(new_bucket, maxBucketLength);
						
						getKeysBucket(new_bucket, maxBucketLength)[new_bucket_length] = e;
						MemCopy(sizeof(T), &getValuesBucket(bucket, maxBucketLength)[element_index], &getValuesBucket(bucket, maxBucketLength)[new_bucket_length]);
						
						MemCopy((current_bucket_length - element_index) * sizeof(key_type), getKeysBucket(bucket, maxBucketLength) + element_index + 1, getKeysBucket(bucket, maxBucketLength) + element_index);
						MemCopy((current_bucket_length - element_index) * sizeof(T), getValuesBucket(bucket, maxBucketLength) + element_index + 1, getValuesBucket(bucket, maxBucketLength) + element_index);

						--current_bucket_length;
						++new_bucket;
					}
					
					++element_index;
				}
			}
		}

		static uint64 getKeyBucketAllocationSize(const uint32 bucketLength) { return (bucketLength + 1) * sizeof(key_type); }
		static uint64 getKeysAllocationSize(const uint32 capacity, const uint32 maxBucketLength) { return getKeyBucketAllocationSize(maxBucketLength) * capacity; }

		static uint64 getValuesVectorAllocationsSize(const uint32 bucketLength) { return bucketLength * sizeof(T); }
		static uint64 getValuesAllocationsSize(const uint32 capacity, const uint32 maxBucketLength) { return getValuesVectorAllocationsSize(maxBucketLength) * capacity; }

		static uint64 getTotalAllocationSize(const uint32 capacity, const uint32 maxBucketLength) { return getKeysAllocationSize(capacity, maxBucketLength) + getValuesAllocationsSize(capacity, maxBucketLength); }

		[[nodiscard]] uint64& getBucketLength(const uint32 index, const uint32 maxBucketLength) const { return *getKeysBucketPointer(index, maxBucketLength); }
		
		byte* allocate(const uint32 newCapacity, const uint32 maxBucketLength)
		{
			uint64 allocated_size{ 0 };	void* memory{ nullptr };
			allocator.Allocate(getTotalAllocationSize(newCapacity, maxBucketLength), alignof(T), &memory, &allocated_size);
			return static_cast<byte*>(memory);
		}

		void deallocate(const uint32 maxBucketLength) const { allocator.Deallocate(getTotalAllocationSize(this->capacity, maxBucketLength), alignof(T), this->data); }

		void build(const uint32 oldCapacity, const uint32 maxBucketLength) { for (uint32 i = oldCapacity; i < this->capacity; ++i) { getBucketLength(i, maxBucketLength) = 0; } }

		void copy(const uint32 newCapacity, byte* to, const uint32 maxBucketLength)
		{
			for (uint32 i = 0; i < this->capacity; ++i)
			{
				MemCopy(getKeysAllocationSize(this->capacity, maxBucketLength), getKeysBucketPointer(i, maxBucketLength), to + getKeyBucketAllocationSize(newCapacity) * i);
				MemCopy(getValuesAllocationsSize(this->capacity, maxBucketLength), getValuesBucketPointer(i, maxBucketLength), to + getKeysAllocationSize(newCapacity, maxBucketLength) + getValuesVectorAllocationsSize(newCapacity) * i);
			}
		}

		[[nodiscard]] key_type* getKeysBucketPointer(const uint32 index, const uint32 maxBucketLength) const { return reinterpret_cast<key_type*>(this->data) + index * (maxBucketLength + 1); }

		T* getValuesBucketPointer(const uint32 bucket, const uint32 maxBucketLength) const { return reinterpret_cast<T*>(this->data + getKeysAllocationSize(this->capacity, maxBucketLength)) + bucket * maxBucketLength; }

		[[nodiscard]] uint32 getMaxBucketLength() const { return static_cast<uint32>(this->capacity * this->loadFactor); }
		
		template<typename TT, class ALLOCATOR, typename L >
		friend void ForEach(FlatHashMap<TT, ALLOCATOR>& collection, L&& lambda);

		template<typename T, class ALLOCATOR>
		friend void Insert(const FlatHashMap<T, ALLOCATOR>&, class Buffer& buffer);
		template<typename T, class ALLOCATOR>
		friend void Extract(FlatHashMap<T, ALLOCATOR>&, class Buffer& buffer);
	};

	template<typename T, class ALLOCATOR, typename L>
	void ForEach(FlatHashMap<T, ALLOCATOR>& collection, L&& lambda)
	{
		for (uint32 bucket = 0; bucket < collection.capacity; ++bucket) { for (auto& e : collection.getValuesBucket(bucket, collection.getMaxBucketLength())) { lambda(e); } }
	}
}