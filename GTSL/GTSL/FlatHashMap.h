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
	template<typename T>
	class FlatHashMap
	{
	public:
		using key_type = uint64;

		FlatHashMap() = default;

		FlatHashMap(const uint32 size, const AllocatorReference& allocatorReference) : capacity(size)
		{
			GTSL_ASSERT(size != 0 && (size & (size - 1)) == 0, "Size is not a power of two!");
			this->data = allocate(size, allocatorReference); build(0);
		}

		void Free(const AllocatorReference& allocatorReference) { deallocate(allocatorReference); this->data = nullptr; }

		~FlatHashMap() { GTSL_ASSERT(!this->data, "Data was not freed!") }

		template<typename T>
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

			T& operator*() const { return *(map->getValuesBucket(bucket) + index); }
			T* operator->() const { return &(map->getValuesBucket(bucket) + index); }

			bool operator==(const Iterator& other) const { return bucket == other.bucket && index == other.index; }

			bool operator!=(const Iterator& other) const { return bucket != other.bucket || index != other.index; }

		private:
			FlatHashMap* map{ nullptr };
			uint32 bucket{ 0 }; uint32 index{ 0 }; uint32 bucketLength{ 0 };
		};

		Iterator<T> begin()
		{
			uint32 bucket{ 0 }, index{ 0 };
			for (uint32 i = 0; i < this->capacity; ++i) { if (this->getBucketLength(i) != 0) { bucket = i; break; } }
			return Iterator<T>(this, bucket, index);
		}
		
		Iterator<T> end()
		{
			uint32 bucket{ 0 }, index{ 0 };
			for (uint32 i = this->capacity; i > 0; --i) { if ((index = this->getBucketLength(i - 1)) != 0) { bucket = i - 1; break; } }
			return Iterator<T>(this, bucket, index);
		}

		template<typename... ARGS>
		T* Emplace(const AllocatorReference& allocatorReference, const key_type key, ARGS&&... args)
		{
			const auto bucket = modulo(key, this->capacity);
			GTSL_ASSERT(findKeyInBucket(bucket, key) == nullptr, "Key already exists!")
			uint64 place_index = getBucketLength(bucket)++;
			if (place_index + 1 > this->capacity) { resize(allocatorReference); }
			getKeysBucket(bucket)[place_index] = key;
			return ::new(getValuesBucket(bucket) + place_index) T(MakeForwardReference<ARGS>(args)...);
		}

		T& At(const key_type key)
		{
			const auto bucket = modulo(key, capacity); const auto index = getIndexForKeyInBucket(bucket, key);
			GTSL_ASSERT(index != 0xFFFFFFFF, "No element with that key!");
			return getValuesBucket(bucket)[index];
		}

		void Remove(const key_type key)
		{
			auto bucket = modulo(key, this->capacity); auto index = getIndexForKeyInBucket(bucket, key);
			key_type bucket_length = getBucketLength(bucket)--;
			GTSL_ASSERT(findKeyInBucket(bucket, key) == nullptr, "Key doesn't exist!")
			getKeysBucket(bucket)[index].~T();
			MemCopy((bucket_length - index) * sizeof(key_type), getKeysBucket(bucket) + index + 1, getKeysBucket(bucket) + index);
			MemCopy((bucket_length - index) * sizeof(T), getValuesBucket(bucket) + index + 1, getValuesBucket(bucket) + index);
		}
		
	private:
		friend class Iterator<T>;

		byte* data = nullptr;
		uint32 capacity = 0;
		uint8 loadFactor = 0;

		[[nodiscard]] key_type* findKeyInBucket(const uint32 bucket, const key_type key) const
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
			
			for (auto& e : getKeysBucket(bucket)) { if (e == key) { return &e; } } return nullptr;
		}

		[[nodiscard]] uint32 getIndexForKeyInBucket(const uint32 bucket, const key_type key) const
		{
			for (auto& e : getKeysBucket(bucket)) { if (e == key) { return &e - getKeysBucket(bucket).begin(); } } return 0xFFFFFFFF;
		}

		[[nodiscard]] Ranger<key_type> getKeysBucket(const uint32 bucket) const { return Ranger<key_type>(getBucketLength(bucket), getKeysBucketPointer(bucket) + 1); }

		[[nodiscard]] Ranger<T> getValuesBucket(const uint32 bucket) const { return Ranger<T>(getBucketLength(bucket), getValuesBucketPointer(bucket)); }

		static constexpr uint32 modulo(const key_type key, const uint32 size) { return key & (size - 1); }

		void resize(const AllocatorReference& allocatorReference)
		{
			auto new_length = this->capacity * 2;
			auto new_alloc = allocate(new_length, allocatorReference);
			copy(new_length, new_alloc);
			deallocate(allocatorReference);
			this->data = new_alloc; this->capacity = new_length;
			build(new_length);
		}

		static uint64 getKeyBucketAllocationSize(const uint32 length) { return (length + 1) * sizeof(key_type); }
		static uint64 getKeysAllocationSize(const uint32 length) { return getKeyBucketAllocationSize(length) * length; }

		static uint64 getValuesVectorAllocationsSize(const uint32 length) { return length * sizeof(T); }
		static uint64 getValuesAllocationsSize(const uint32 length) { return getValuesVectorAllocationsSize(length) * length; }

		static uint64 getTotalAllocationSize(const uint32 length) { return getKeysAllocationSize(length) + getValuesAllocationsSize(length); }

		static uint64& getBucketLength(byte* data, const uint32 capacity, const uint32 index) { return *(reinterpret_cast<key_type*>(data) + ((capacity + 1) * index)); }

		byte* allocate(const uint64 newLength, const AllocatorReference& allocatorReference)
		{
			uint64 allocated_size{ 0 };	void* memory{ nullptr };
			allocatorReference.Allocate(getTotalAllocationSize(newLength), alignof(T), &memory, &allocated_size);
			return static_cast<byte*>(memory);
		}

		void deallocate(const AllocatorReference& allocatorReference) const { allocatorReference.Deallocate(getTotalAllocationSize(this->capacity), alignof(T), this->data); }

		void build(const uint32 oldLength) { for (uint32 i = oldLength; i < this->capacity; ++i) { getBucketLength(i) = 0; } }

		void copy(const uint64 newLength, byte* to)
		{
			for (uint32 i = 0; i < this->capacity; ++i)
			{
				MemCopy(getKeysAllocationSize(this->capacity), getKeysBucketPointer(i), to + getKeyBucketAllocationSize(newLength) * i);
				MemCopy(getValuesAllocationsSize(this->capacity), getValuesBucketPointer(i), to + getKeysAllocationSize(newLength) + getValuesVectorAllocationsSize(newLength) * i);
			}
		}

		[[nodiscard]] key_type* getKeysBucketPointer(const uint32 index) const { return &getBucketLength(this->data, this->capacity, index); }

		[[nodiscard]] uint64& getBucketLength(const uint32 index) const { return getBucketLength(this->data, this->capacity, index); }

		T* getValuesBucketPointer(const uint32 index) const { return reinterpret_cast<T*>(this->data + getKeysAllocationSize(this->capacity)) + index * this->capacity; }

		template<typename TT, typename L>
		friend void ForEach(FlatHashMap<TT>& collection, L&& lambda);
	};

	template<typename T, typename L>
	void ForEach(FlatHashMap<T>& collection, L&& lambda)
	{
		for (uint32 bucket = 0; bucket < collection.capacity; ++bucket) { for (auto& e : collection.getValuesBucket(bucket)) { lambda(e); } }
	}
}