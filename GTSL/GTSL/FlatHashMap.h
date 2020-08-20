#pragma once

#include "Core.h"

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

		FlatHashMap(const uint32 size, const ALLOCATOR& allocatorReference) : capacity(size), maxBucketLength(size / 2), allocator(allocatorReference)
		{
			GTSL_ASSERT(size != 0 && (size & (size - 1)) == 0, "Size is not a power of two!");
			GTSL_ASSERT(this->maxBucketLength != 0, "Size and load factor combination leads to empty buckets!")
			this->data = allocate(size, maxBucketLength); build(0);
		}

		FlatHashMap(const uint32 size, const float32 loadFactor, const ALLOCATOR& allocatorReference) : capacity(size), maxBucketLength(size * loadFactor), allocator(allocatorReference)
		{
			GTSL_ASSERT(size != 0 && (size & (size - 1)) == 0, "Size is not a power of two!");
			GTSL_ASSERT(this->maxBucketLength != 0, "Size and load factor combination leads to empty buckets!");
			this->data = allocate(size, maxBucketLength); build(0);
		}

		FlatHashMap(FlatHashMap&& other) noexcept : data(other.data), capacity(other.capacity), maxBucketLength(other.maxBucketLength), allocator(MoveRef(other.allocator))
		{
			other.data = nullptr; other.capacity = 0;
		}

		void Initialize(const uint32 size, const ALLOCATOR& allocatorReference)
		{
			GTSL_ASSERT(size != 0 && (size & (size - 1)) == 0, "Size is not a power of two!");
			this->capacity = size;
			this->allocator = allocatorReference;
			this->maxBucketLength = size / 2;
			this->data = allocate(size, maxBucketLength);
			build(0);
		}
		
		~FlatHashMap()
		{
			if (this->data)
			{			
				for(uint32 i = 0; i < this->capacity; ++i)
				{
					for(auto& e : this->getValuesBucket(i)) { e.~T(); }
				}
				
				deallocate();
				this->data = nullptr;
			}
		}

		template<typename TT>
		class Iterator
		{
		public:
			Iterator(FlatHashMap* hashMap) : map(hashMap)
			{
			}

			Iterator(FlatHashMap* hashMap, uint32 vector, uint32 index) : map(hashMap), bucketIndex(vector), index(index), bucketLength(map->getBucketLength(this->bucketIndex))
			{
			}

			Iterator operator++()
			{
				auto range = map->getValuesBucket(bucketIndex);

				if (index + 1 < bucketLength)
				{
					++index;
				}
				else //if bucket is over
				{
					for (uint32 i = bucketIndex + 1; i < map->capacity; ++i) //search for next non empty bucket
					{
						if ((bucketLength = map->getBucketLength(i)) != 0) //vector with some element/s
						{
							bucketIndex = i; index = 0; return (*this);
						}
					}

					++index;
				}

				return (*this);
			}

			Iterator operator--()
			{
				auto range = map->getValuesBucket(bucketIndex);
				
				if (range.begin() + index == range.begin())
				{
					--bucketIndex; index = 0;
				}
				else
				{
					--index;
				}

				return (*this);
			}

			Iterator operator++(int) { Iterator ret = *this; ++(*this); return ret; }
			Iterator operator--(int) { Iterator ret = *this; --(*this); return ret; }

			TT& operator*() const { return *(map->getValuesBucket(bucketIndex) + index); }
			TT* operator->() const { return map->getValuesBucket(bucketIndex) + index; }

			bool operator==(const Iterator& other) const { return bucketIndex == other.bucketIndex && index == other.index; }

			bool operator!=(const Iterator& other) const { return bucketIndex != other.bucketIndex || index != other.index; }

		private:
			FlatHashMap* map{ nullptr };
			uint32 bucketIndex{ 0 }; uint32 index{ 0 }; uint32 bucketLength{ 0 };
		};

		Iterator<T> begin()
		{
			uint32 bucketIndex{ 0 }, index{ 0 };
			for (uint32 i = 0; i < this->capacity; ++i) { if (this->getBucketLength(i) != 0) { bucketIndex = i; break; } }
			return Iterator<T>(this, bucketIndex, index);
		}
		
		Iterator<T> end()
		{
			uint32 bucketIndex{ 0 }, index{ 0 };
			for (uint32 i = this->capacity; i > 0; --i) { if ((index = this->getBucketLength(i - 1)) != 0) { bucketIndex = i - 1; break; } }
			return Iterator<T>(this, bucketIndex, index);
		}

		template<typename... ARGS>
		T& Emplace(const key_type key, ARGS&&... args)
		{
			auto bucketIndex = modulo(key, this->capacity);
			GTSL_ASSERT(findKeyInBucket(bucketIndex, key) == nullptr, "Key already exists!")
			
			uint64 placeIndex = getBucketLength(bucketIndex);
			if (placeIndex + 1 > maxBucketLength)
			{
				resize();
				bucketIndex = modulo(key, this->capacity);
				placeIndex = getBucketLength(bucketIndex)++;
			}
			else
			{
				++getBucketLength(bucketIndex);
			}
			
			getKeysBucket(bucketIndex)[placeIndex] = key;
			return *new(getValuesBucket(bucketIndex) + placeIndex) T(ForwardRef<ARGS>(args)...);
		}

		bool Find(const key_type key) { return findKeyInBucket(modulo(key, this->capacity), key); }
		
		bool Find(const key_type key, T*& obj)
		{
			const auto bucket = modulo(key, this->capacity);
			auto elementIndex = getIndexForKeyInBucket(bucket, key);
			obj = getValuesBucketPointer(bucket) + elementIndex;
			return elementIndex != 0xFFFFFFFF;
		}
		
		T& At(const key_type key)
		{
			const auto bucketIndex = modulo(key, capacity); const auto elementIndex = getIndexForKeyInBucket(bucketIndex, key);
			GTSL_ASSERT(elementIndex != 0xFFFFFFFF, "No element with that key!");
			return getValuesBucket(bucketIndex)[elementIndex];
		}

		void Remove(const key_type key)
		{
			auto bucketIndex = modulo(key, this->capacity); auto elementIndex = getIndexForKeyInBucket(bucketIndex, key);
			key_type bucketLength = getBucketLength(bucketIndex)--;
			GTSL_ASSERT(elementIndex != 0xFFFFFFFF, "Key doesn't exist!")
			getValuesBucket(bucketIndex)[elementIndex].~T();
			MemCopy((bucketLength - elementIndex) * sizeof(key_type), getKeysBucket(bucketIndex) + elementIndex + 1, getKeysBucket(bucketIndex) + elementIndex);
			MemCopy((bucketLength - elementIndex) * sizeof(T), getValuesBucket(bucketIndex) + elementIndex + 1, getValuesBucket(bucketIndex) + elementIndex);
		}
		
	private:
		friend class Iterator<T>;

		byte* data = nullptr;
		uint32 capacity = 0;
		uint32 maxBucketLength = 0;
		[[no_unique_address]] ALLOCATOR allocator;

		[[nodiscard]] key_type* findKeyInBucket(const uint32 keys_bucket, const key_type key) const
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
			
			for (auto& e : getKeysBucket(keys_bucket)) { if (e == key) { return &e; } } return nullptr;
		}

		[[nodiscard]] uint32 getIndexForKeyInBucket(const uint32 keys_bucket, const key_type key) const
		{
			for (auto& e : getKeysBucket(keys_bucket)) { if (e == key) { return static_cast<uint32>(&e - getKeysBucket(keys_bucket).begin()); } } return 0xFFFFFFFF;
		}

		[[nodiscard]] Ranger<key_type> getKeysBucket(const uint32 bucketIndex) const { return Ranger<key_type>(getBucketLength(bucketIndex), getKeysBucketPointer(bucketIndex) + 1); }

		[[nodiscard]] Ranger<T> getValuesBucket(const uint32 keys_bucket) const { return Ranger<T>(getBucketLength(keys_bucket), getValuesBucketPointer(keys_bucket)); }

		static constexpr uint32 modulo(const key_type key, const uint32 size) { return key & (size - 1); }

		void resize()
		{
			auto newCapacity = this->capacity * 2;
			auto newMaxBucketLength = this->maxBucketLength * 2;
			auto* newAlloc = allocate(newCapacity, newMaxBucketLength);
			copy(newCapacity, newAlloc);
			deallocate();
			this->data = newAlloc;
			this->capacity = newCapacity;
			this->maxBucketLength = newMaxBucketLength;
			build(this->capacity / 2);

			for(uint32 bucketIndex = 0; bucketIndex < this->capacity / 2; ++bucketIndex) //rehash lower half only since it's the only part that has elements
			{
				uint32 elementIndex = 0;
				
				for(auto e : getKeysBucket(bucketIndex))
				{
					auto newBucket = modulo(e, this->capacity);
					
					if(newBucket != bucketIndex) //If after resizing, element would not still be in the same bucket, move it || Most elements keep their original modulo
					{	
						auto& currentBucketLength = getBucketLength(bucketIndex); auto& newBucketLength = getBucketLength(newBucket);
						
						getKeysBucket(newBucket)[newBucketLength] = e;
						MemCopy(sizeof(T), &getValuesBucket(bucketIndex)[elementIndex], &getValuesBucket(bucketIndex)[newBucketLength]);
						
						MemCopy((currentBucketLength - elementIndex) * sizeof(key_type), getKeysBucket(bucketIndex) + elementIndex + 1, getKeysBucket(bucketIndex) + elementIndex);
						MemCopy((currentBucketLength - elementIndex) * sizeof(T), getValuesBucket(bucketIndex) + elementIndex + 1, getValuesBucket(bucketIndex) + elementIndex);

						--currentBucketLength;
						++newBucket;
					}
					
					++elementIndex;
				}
			}
		}

		static uint64 getKeyBucketAllocationSize(const uint32 bucketLength) { return (bucketLength + 1) * sizeof(key_type); }
		static uint64 getKeysAllocationSize(const uint32 capacity, const uint32 maxBucketLength) { return getKeyBucketAllocationSize(maxBucketLength) * capacity; }

		static uint64 getValuesVectorAllocationsSize(const uint32 bucketLength) { return bucketLength * sizeof(T); }
		static uint64 getValuesAllocationsSize(const uint32 capacity, const uint32 maxBucketLength) { return getValuesVectorAllocationsSize(maxBucketLength) * capacity; }

		static uint64 getTotalAllocationSize(const uint32 capacity, const uint32 maxBucketLength) { return getKeysAllocationSize(capacity, maxBucketLength) + getValuesAllocationsSize(capacity, maxBucketLength); }

		[[nodiscard]] uint64& getBucketLength(const uint32 index) const { return *getKeysBucketPointer(index); }
		
		byte* allocate(const uint32 newCapacity, const uint32 newMaxBucketLength)
		{
			uint64 allocated_size{ 0 };	void* memory{ nullptr };
			allocator.Allocate(getTotalAllocationSize(newCapacity, newMaxBucketLength), alignof(T), &memory, &allocated_size);
			return static_cast<byte*>(memory);
		}

		void deallocate() const { allocator.Deallocate(getTotalAllocationSize(this->capacity, maxBucketLength), alignof(T), this->data); }

		void build(const uint32 oldCapacity) { for (uint32 i = oldCapacity; i < this->capacity; ++i) { getBucketLength(i) = 0; } }

		void copy(const uint32 newCapacity, byte* to)
		{
			for (uint32 i = 0; i < this->capacity; ++i)
			{
				MemCopy(getKeysAllocationSize(this->capacity, maxBucketLength), getKeysBucketPointer(i), to + getKeyBucketAllocationSize(newCapacity) * i);
				MemCopy(getValuesAllocationsSize(this->capacity, maxBucketLength), getValuesBucketPointer(i), to + getKeysAllocationSize(newCapacity, maxBucketLength) + getValuesVectorAllocationsSize(newCapacity) * i);
			}
		}

		[[nodiscard]] key_type* getKeysBucketPointer(const uint32 index) const { return reinterpret_cast<key_type*>(this->data) + index * (maxBucketLength + 1); }

		T* getValuesBucketPointer(const uint32 keys_bucket) const { return reinterpret_cast<T*>(this->data + getKeysAllocationSize(this->capacity, maxBucketLength)) + keys_bucket * maxBucketLength; }
		
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
		for (uint32 bucketIndex = 0; bucketIndex < collection.capacity; ++bucketIndex) { for (auto& e : collection.getValuesBucket(bucketIndex)) { lambda(e); } }
	}
}