#pragma once

#include "Core.h"

#include "Memory.h"
#include "Range.h"
#include <new>
#include "ArrayCommon.hpp"

#if (_DEBUG)
#include "Assert.h"
#endif

namespace GTSL
{
	template<typename T, Allocator ALLOCATOR>
	class FlatHashMap
	{
	public:
		using key_type = uint64;

		FlatHashMap() = default;

		FlatHashMap(const uint32 size, const ALLOCATOR& allocatorReference) : capacity(size), maxBucketLength(size / 2), allocator(allocatorReference)
		{
			GTSL_ASSERT(size != 0 && (size & (size - 1)) == 0, "Size is not a power of two!");
			GTSL_ASSERT(this->maxBucketLength != 0, "Size and load factor combination leads to empty buckets!")
				this->data = allocate(size, maxBucketLength); build();
		}

		FlatHashMap(const uint32 size, const float32 loadFactor, const ALLOCATOR& allocatorReference) : capacity(size), maxBucketLength(size* loadFactor), allocator(allocatorReference)
		{
			GTSL_ASSERT(size != 0 && (size & (size - 1)) == 0, "Size is not a power of two!");
			GTSL_ASSERT(this->maxBucketLength != 0, "Size and load factor combination leads to empty buckets!");
			this->data = allocate(size, maxBucketLength); build();
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
			build();
		}

		~FlatHashMap()
		{
			if (this->data)
			{
				for (uint32 i = 0; i < this->capacity; ++i)
				{
					for (auto& e : this->getValuesBucket(i)) { e.~T(); }
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

		Iterator<const T> begin() const
		{
			uint32 bucketIndex{ 0 }, index{ 0 };
			for (uint32 i = 0; i < this->capacity; ++i) { if (this->getBucketLength(i) != 0) { bucketIndex = i; break; } }
			return Iterator<const T>(this, bucketIndex, index);
		}

		Iterator<const T> end() const
		{
			uint32 bucketIndex{ 0 }, index{ 0 };
			for (uint32 i = this->capacity; i > 0; --i) { if ((index = this->getBucketLength(i - 1)) != 0) { bucketIndex = i - 1; break; } }
			return Iterator<const T>(this, bucketIndex, index);
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
			return *new(getValuesBucket(bucketIndex).begin() + placeIndex) T(ForwardRef<ARGS>(args)...);
		}

		[[nodiscard]] bool Find(const key_type key) const { return findKeyInBucket(modulo(key, this->capacity), key); }

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
			getValuesBucket(bucketIndex)[elementIndex].~T();
			GTSL_ASSERT(elementIndex != 0xFFFFFFFF, "Key doesn't exist!")
			popElement(getKeysBucket(bucketIndex), elementIndex);
			popElement(getValuesBucket(bucketIndex), elementIndex);
			--getBucketLength(bucketIndex);
			//MemCopy((bucketLength - elementIndex) * sizeof(key_type), getKeysBucket(bucketIndex) + elementIndex + 1, getKeysBucket(bucketIndex) + elementIndex);
			//MemCopy((bucketLength - elementIndex) * sizeof(T), getValuesBucket(bucketIndex) + elementIndex + 1, getValuesBucket(bucketIndex) + elementIndex);
		}

		void Clear()
		{
			for (uint32 bucket = 0; bucket < this->capacity; ++bucket) { for (auto& e : getValuesBucket(bucket)) { e.~T(); } }
			for (uint32 bucket = 0; bucket < this->capacity; ++bucket) { getBucketLength(bucket) = 0; }
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

		[[nodiscard]] Range<key_type*> getKeysBucket(const uint32 bucketIndex) const { return Range<key_type*>(getBucketLength(bucketIndex), getKeysBucketPointer(bucketIndex) + 1); }
		[[nodiscard]] Range<key_type*> getKeysBucket(const uint32 bucketIndex, const uint32 length) const { return Range<key_type*>(getBucketLength(bucketIndex, length), getKeysBucketPointer(bucketIndex, length) + 1); }

		[[nodiscard]] Range<T*> getValuesBucket(const uint32 bucketIndex) const { return Range<T*>(getBucketLength(bucketIndex), getValuesBucketPointer(bucketIndex)); }
		[[nodiscard]] Range<T*> getValuesBucket(const uint32 bucketIndex, const uint32 length) const { return Range<T*>(getBucketLength(bucketIndex, length), getValuesBucketPointer(bucketIndex, length)); }

		static constexpr uint32 modulo(const key_type key, const uint32 size) { return key & (size - 1); }

		void resize()
		{
			auto newCapacity = this->capacity * 2;
			auto newMaxBucketLength = this->maxBucketLength * 2;
			auto* newAlloc = allocate(newCapacity, newMaxBucketLength);
			copy(newCapacity, newMaxBucketLength, newAlloc);
			deallocate();
			this->data = newAlloc;
			this->capacity = newCapacity;
			this->maxBucketLength = newMaxBucketLength;
			build(this->capacity / 2);

			for (uint32 currentBucketIndex = 0; currentBucketIndex < this->capacity / 2; ++currentBucketIndex) //rehash lower half only since it's the only part that has elements
			{
				for (auto* key = getKeysBucket(currentBucketIndex).begin(); key != getKeysBucket(currentBucketIndex).end(); ++key)
				{
					uint32 elementIndex = key - getKeysBucket(currentBucketIndex).begin();
					uint32 bcktLen = getKeysBucket(currentBucketIndex).ElementCount();

					auto newBucketIndex = modulo(*key, this->capacity);

					if (newBucketIndex != currentBucketIndex) //If after resizing, element would not still be in the same bucket, move it || Most elements keep their original modulo
					{
						auto& currentBucketLength = getBucketLength(currentBucketIndex); auto& newBucketLength = getBucketLength(newBucketIndex);

						if (newBucketLength + 1 > newMaxBucketLength) { resize(); }

						*(getKeysBucket(newBucketIndex).begin() + static_cast<key_type>(newBucketLength)) = *key; //move key
						popElement(getKeysBucket(currentBucketIndex), elementIndex); //pop key

						copyElementToBack(getValuesBucket(newBucketIndex), getValuesBucket(currentBucketIndex).begin() + elementIndex); //move object
						popElement(getValuesBucket(currentBucketIndex), elementIndex); //pop object

						//MemCopy((currentBucketLength - elementIndex) * sizeof(key_type), getKeysBucket(currentBucketIndex) + elementIndex + 1, getKeysBucket(currentBucketIndex) + elementIndex); //
						//MemCopy(sizeof(T), getValuesBucket(currentBucketIndex) + elementIndex, getValuesBucket(newBucketIndex) + newBucketPos); //move element to new bucket
						//MemCopy((currentBucketLength - elementIndex) * sizeof(T), getValuesBucket(currentBucketIndex) + elementIndex + 1, getValuesBucket(currentBucketIndex) + elementIndex);

						--currentBucketLength;
						++newBucketLength;
					}
				}
			}

		}

		static uint64 getKeysBucketAllocationSize(const uint32 bucketLength) { return (bucketLength + 1) * sizeof(key_type); }
		static uint64 getKeysAllocationSize(const uint32 capacity, const uint32 maxBucketLength) { return getKeysBucketAllocationSize(maxBucketLength) * capacity; }

		static uint64 getValuesBucketAllocationsSize(const uint32 bucketLength) { return bucketLength * sizeof(T); }
		static uint64 getValuesAllocationsSize(const uint32 capacity, const uint32 maxBucketLength) { return getValuesBucketAllocationsSize(maxBucketLength) * capacity; }

		static uint64 getTotalAllocationSize(const uint32 capacity, const uint32 maxBucketLength) { return getKeysAllocationSize(capacity, maxBucketLength) + getValuesAllocationsSize(capacity, maxBucketLength); }

		[[nodiscard]] uint64& getBucketLength(const uint32 index) const { return *getKeysBucketPointer(index); }
		[[nodiscard]] uint64& getBucketLength(const uint32 index, const uint32 length) const { return *getKeysBucketPointer(index); }

		byte* allocate(const uint32 newCapacity, const uint32 newMaxBucketLength)
		{
			uint64 allocated_size{ 0 };	void* memory{ nullptr };
			allocator.Allocate(getTotalAllocationSize(newCapacity, newMaxBucketLength), alignof(T), &memory, &allocated_size);
			return static_cast<byte*>(memory);
		}

		void deallocate() const { allocator.Deallocate(getTotalAllocationSize(this->capacity, maxBucketLength), alignof(T), this->data); }

		void build() { for (uint32 i = 0; i < this->capacity; ++i) { getBucketLength(i) = 0; } }
		void build(const uint32 oldCapacity) { for (uint32 i = oldCapacity - 1; i < this->capacity; ++i) { getBucketLength(i) = 0; } }

		void copy(const uint32 newCapacity, const uint32 newMaxBucketLength, byte* to)
		{
			for (uint32 i = 0; i < this->capacity; ++i)
			{
				MemCopy(getKeysBucketAllocationSize(maxBucketLength), getKeysBucketPointer(i), getKeysBucketPointer(to, i, newMaxBucketLength));
				MemCopy(getValuesBucketAllocationsSize(maxBucketLength), getValuesBucketPointer(i), getValuesBucketPointer(to, i, newMaxBucketLength, newCapacity));
			}
		}

		[[nodiscard]] key_type* getKeysBucketPointer(const uint32 index) const { return reinterpret_cast<key_type*>(this->data) + index * (maxBucketLength + 1); }
		[[nodiscard]] key_type* getKeysBucketPointer(const uint32 index, const uint32 length) const { return reinterpret_cast<key_type*>(this->data) + index * (length + 1); }
		[[nodiscard]] key_type* getKeysBucketPointer(byte* to, const uint32 index, const uint32 length) const { return reinterpret_cast<key_type*>(to) + index * (length + 1); }

		T* getValuesBucketPointer(const uint32 bucketIndex) const { return reinterpret_cast<T*>(this->data + getKeysAllocationSize(this->capacity, maxBucketLength)) + bucketIndex * maxBucketLength; }
		T* getValuesBucketPointer(const uint32 bucketIndex, const uint32 length) const { return reinterpret_cast<T*>(this->data + getKeysAllocationSize(this->capacity, length)) + bucketIndex * length; }
		T* getValuesBucketPointer(byte* to, const uint32 bucketIndex, const uint32 length, const uint32 cap) const { return reinterpret_cast<T*>(to + getKeysAllocationSize(cap, length)) + bucketIndex * length; }

		template<typename TT, class ALLOCATOR, typename L>
		friend void ForEach(FlatHashMap<TT, ALLOCATOR>& collection, L&& lambda);

		template<typename TT, class ALLOCATOR, typename L>
		friend void ForEach(const FlatHashMap<TT, ALLOCATOR>& collection, L&& lambda);

		template<typename TT, class ALLOCATOR, typename L>
		friend void PairForEach(FlatHashMap<TT, ALLOCATOR>& collection, L&& lambda);

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

	template<typename T, class ALLOCATOR, typename L>
	void ForEach(const FlatHashMap<T, ALLOCATOR>& collection, L&& lambda)
	{
		for (uint32 bucketIndex = 0; bucketIndex < collection.capacity; ++bucketIndex) { for (const auto& e : collection.getValuesBucket(bucketIndex)) { lambda(e); } }
	}

	template<typename T, class ALLOCATOR, typename L>
	void PairForEach(FlatHashMap<T, ALLOCATOR>& collection, L&& lambda)
	{
		for (uint32 bucketIndex = 0; bucketIndex < collection.capacity; ++bucketIndex)
		{
			for (uint32 i = 0; i < collection.getKeysBucket(bucketIndex).ElementCount(); ++i)
			{
				lambda(collection.getKeysBucket(bucketIndex)[i], collection.getValuesBucket(bucketIndex)[i]);
			}
		}
	}
}