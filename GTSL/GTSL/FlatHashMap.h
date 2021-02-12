#pragma once

#include "Core.h"

#include "Memory.h"
#include "Range.h"
#include <new>
#include "ArrayCommon.hpp"
#include "Bitman.h"
#include "Result.h"

#if (_DEBUG)
#include "Assert.h"
#endif

namespace GTSL
{
	template<class ALLOCATOR>
	class Buffer;
	
	template<typename T, Allocator ALLOCATOR>
	class FlatHashMap
	{
	public:
		using key_type = uint64;

		FlatHashMap() = default;

		FlatHashMap(const uint32 size, const ALLOCATOR& allocatorReference) : bucketCount(NextPowerOfTwo(size)), maxBucketLength(bucketCount / 2), allocator(allocatorReference)
		{
			while (maxBucketLength == 0) {
				bucketCount *= 2;
				maxBucketLength = bucketCount / 2;
			}
			
			this->data = allocate(bucketCount, maxBucketLength); initializeBuckets();
		}

		FlatHashMap(const uint32 size, const float32 loadFactor, const ALLOCATOR& allocatorReference) : bucketCount(NextPowerOfTwo(size)), maxBucketLength(bucketCount * loadFactor), allocator(allocatorReference)
		{
			while (maxBucketLength == 0) {
				bucketCount *= 2;
				maxBucketLength = bucketCount / 2;
			}
			
			this->data = allocate(bucketCount, maxBucketLength); initializeBuckets();
		}

		FlatHashMap(FlatHashMap&& other) noexcept : data(other.data), bucketCount(other.bucketCount), maxBucketLength(other.maxBucketLength), allocator(MoveRef(other.allocator))
		{
			other.data = nullptr; other.bucketCount = 0;
		}

		void Initialize(const uint32 size, const ALLOCATOR& allocatorReference)
		{
			this->bucketCount = NextPowerOfTwo(size);
			this->allocator = allocatorReference;
			this->maxBucketLength = bucketCount / 2;

			while (maxBucketLength == 0) {
				bucketCount *= 2;
				maxBucketLength = bucketCount / 2;
			}
			
			this->data = allocate(bucketCount, maxBucketLength);
			initializeBuckets();
		}

		void Free()
		{
			for (uint32 i = 0; i < this->bucketCount; ++i) {
				for (auto& e : this->getValuesBucket(i)) { e.~T(); }
			}

			deallocate();
			this->data = nullptr;
		}
		
		~FlatHashMap()
		{
			if (this->data) { Free(); }
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
					for (uint32 i = bucketIndex + 1; i < map->bucketCount; ++i) //search for next non empty bucket
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

			TT& operator*() const { return map->getValuesBucket(bucketIndex)[index]; }
			TT* operator->() const { return map->getValuesBucket(bucketIndex).begin() + index; }

			bool operator==(const Iterator& other) const { return bucketIndex == other.bucketIndex && index == other.index; }

			bool operator!=(const Iterator& other) const { return bucketIndex != other.bucketIndex || index != other.index; }

		private:
			FlatHashMap* map{ nullptr };
			uint32 bucketIndex{ 0 }; uint32 index{ 0 }; uint32 bucketLength{ 0 };
		};

		Iterator<T> begin()
		{
			uint32 bucketIndex{ 0 }, index{ 0 };
			for (uint32 i = 0; i < this->bucketCount; ++i) { if (this->getBucketLength(i) != 0) { bucketIndex = i; break; } }
			return Iterator<T>(this, bucketIndex, index);
		}

		Iterator<T> end()
		{
			uint32 bucketIndex{ 0 }, index{ 0 };
			for (uint32 i = this->bucketCount; i > 0; --i) { if ((index = this->getBucketLength(i - 1)) != 0) { bucketIndex = i - 1; break; } }
			return Iterator<T>(this, bucketIndex, index);
		}

		Iterator<const T> begin() const
		{
			uint32 bucketIndex{ 0 }, index{ 0 };
			for (uint32 i = 0; i < this->bucketCount; ++i) { if (this->getBucketLength(i) != 0) { bucketIndex = i; break; } }
			return Iterator<const T>(this, bucketIndex, index);
		}

		Iterator<const T> end() const
		{
			uint32 bucketIndex{ 0 }, index{ 0 };
			for (uint32 i = this->bucketCount; i > 0; --i) { if ((index = this->getBucketLength(i - 1)) != 0) { bucketIndex = i - 1; break; } }
			return Iterator<const T>(this, bucketIndex, index);
		}

		template<typename... ARGS>
		T& Emplace(const key_type key, ARGS&&... args)
		{
			auto bucketIndex = modulo(key, this->bucketCount);
			GTSL_ASSERT(findKeyInBucket(bucketIndex, key) == nullptr, "Key already exists!")

			uint64 placeIndex = getBucketLength(bucketIndex);
			
			if (placeIndex + 1 > maxBucketLength)
			{
				resize();
				bucketIndex = modulo(key, this->bucketCount);
				placeIndex = getBucketLength(bucketIndex)++;
			}
			else
			{
				++getBucketLength(bucketIndex);
			}

			getKeysBucket(bucketIndex)[placeIndex] = key;
			return *new(getValuesBucket(bucketIndex).begin() + placeIndex) T(ForwardRef<ARGS>(args)...);
		}

		template<typename... ARGS>
		GTSL::Result<T&> TryEmplace(const key_type key, ARGS&&... args)
		{
			auto result = TryGet(key);

			if(result.State()) {
				return GTSL::Result<T&>(result.Get(), false);
			}
			else {
				return GTSL::Result<T&>(Emplace(key, GTSL::ForwardRef<ARGS>(args)...), true);
			}
		}

		[[nodiscard]] bool Find(const key_type key) const { return findKeyInBucket(modulo(key, this->bucketCount), key); }

		[[nodiscard]] Result<T&> TryGet(const key_type key)
		{
			const auto bucket = modulo(key, this->bucketCount);
			auto result = getIndexForKeyInBucket(bucket, key);
			return GTSL::Result<T&>(*(getValuesBucketPointer(bucket) + result.Get()), result.State());
		}

		T& At(const key_type key)
		{
			const auto bucketIndex = modulo(key, bucketCount); const auto elementIndex = getIndexForKeyInBucket(bucketIndex, key);
			GTSL_ASSERT(elementIndex.State(), "No element with that key!");
			return getValuesBucket(bucketIndex)[elementIndex.Get()];
		}

		const T& At(const key_type key) const
		{
			const auto bucketIndex = modulo(key, bucketCount); const auto elementIndex = getIndexForKeyInBucket(bucketIndex, key);
			GTSL_ASSERT(elementIndex.State(), "No element with that key!");
			return getValuesBucket(bucketIndex)[elementIndex.Get()];
		}

		void Remove(const key_type key)
		{
			auto bucketIndex = modulo(key, this->bucketCount); auto elementIndex = getIndexForKeyInBucket(bucketIndex, key);
			getValuesBucket(bucketIndex)[elementIndex.Get()].~T();
			GTSL_ASSERT(elementIndex.State(), "Key doesn't exist!")
			popElement(getKeysBucket(bucketIndex), elementIndex.Get());
			popElement(getValuesBucket(bucketIndex), elementIndex.Get());
			--getBucketLength(bucketIndex);
		}

		void Clear()
		{
			for (uint32 bucket = 0; bucket < this->bucketCount; ++bucket) { for (auto& e : getValuesBucket(bucket)) { e.~T(); } }
			for (uint32 bucket = 0; bucket < this->bucketCount; ++bucket) { getBucketLength(bucket) = 0; }
		}

		T& operator[](const uint64 key) { return At(key); }
		const T& operator[](const uint64 key) const { return At(key); }
	
	private:
		friend class Iterator<T>;

		byte* data = nullptr;
		uint32 bucketCount = 0;
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

		[[nodiscard]] GTSL::Result<uint32> getIndexForKeyInBucket(const uint32 bucketIndex, const key_type key) const
		{
			for (auto& e : getKeysBucket(bucketIndex)) { if (e == key) { return GTSL::Result<uint32>(static_cast<uint32>(&e - getKeysBucket(bucketIndex).begin()), true); } }
			return GTSL::Result<uint32>(0, false);
		}

		[[nodiscard]] Range<key_type*> getKeysBucket(const uint32 bucketIndex) const { return Range<key_type*>(getBucketLength(bucketIndex), getKeysBucketPointer(bucketIndex) + 1); }
		[[nodiscard]] Range<key_type*> getKeysBucket(const uint32 bucketIndex, const uint32 length) const { return Range<key_type*>(getBucketLength(bucketIndex, length), getKeysBucketPointer(bucketIndex, length) + 1); }

		[[nodiscard]] Range<T*> getValuesBucket(const uint32 bucketIndex) const { return Range<T*>(getBucketLength(bucketIndex), getValuesBucketPointer(bucketIndex)); }
		[[nodiscard]] Range<T*> getValuesBucket(const uint32 bucketIndex, const uint32 length) const { return Range<T*>(getBucketLength(bucketIndex, length), getValuesBucketPointer(bucketIndex, length)); }

		static constexpr uint32 modulo(const key_type key, const uint32 size) { return key & (size - 1); }

		void resize()
		{
			auto newBucketCount = this->bucketCount * 2;
			auto newMaxBucketLength = this->maxBucketLength * 2;
			auto* newAlloc = allocate(newBucketCount, newMaxBucketLength);
			copy(newBucketCount, newMaxBucketLength, newAlloc);
			deallocate();
			this->data = newAlloc; this->bucketCount = newBucketCount; this->maxBucketLength = newMaxBucketLength;
			initializeBuckets(this->bucketCount / 2 /*old capacity*/); //intialize new upper half

			for (uint32 currentBucketIndex = 0; currentBucketIndex < this->bucketCount / 2; ++currentBucketIndex) //rehash lower half only since it's the only part that has elements
			{
				// Do rehash in reverse because elements at the top of the bucket require less data copying when moved, as we only have to move those elements on top of them
				for (uint32 currentElementIndex = getBucketLength(currentBucketIndex) - 1, i = 0; i < getBucketLength(currentBucketIndex); ++i, --currentElementIndex)
				{
					auto newBucketIndex = modulo(getKeysBucket(currentBucketIndex)[currentElementIndex], this->bucketCount);

					if (newBucketIndex != currentBucketIndex) //If after rehash element has to go to a new bucket, move it (elements tend keep their original modulo, specially as the map grows)
					{
						auto& currentBucketLength = getBucketLength(currentBucketIndex); auto& newBucketLength = getBucketLength(newBucketIndex);

						if (newBucketLength + 1 > newMaxBucketLength) { resize(); return; } //if pushing one more element into the current bucket would exhaust it's space trigger another resize

						copyElementToBack(getKeysBucket(newBucketIndex), getKeysBucket(currentBucketIndex).begin() + currentElementIndex); //move key to new bucket
						popElement(getKeysBucket(currentBucketIndex), currentElementIndex); //pop key from old bucket

						copyElementToBack(getValuesBucket(newBucketIndex), getValuesBucket(currentBucketIndex).begin() + currentElementIndex); //move object to new bucket
						popElement(getValuesBucket(currentBucketIndex), currentElementIndex); //pop object from old bucket

						--currentBucketLength; ++newBucketLength; //update count to match changes
					}
				}
			}
		}

		static uint64 getKeysBucketAllocationSize(const uint32 bucketLength) { return (bucketLength + 1) * sizeof(key_type); }
		static uint64 getKeysAllocationSize(const uint32 bucketCount, const uint32 maxBucketLength) { return getKeysBucketAllocationSize(maxBucketLength) * bucketCount; }

		static uint64 getValuesBucketAllocationsSize(const uint32 bucketLength) { return bucketLength * sizeof(T); }
		static uint64 getValuesAllocationsSize(const uint32 bucketCount, const uint32 maxBucketLength) { return getValuesBucketAllocationsSize(maxBucketLength) * bucketCount; }

		static uint64 getTotalAllocationSize(const uint32 bucketCount, const uint32 bucketLength) { return getKeysAllocationSize(bucketCount, bucketLength) + getValuesAllocationsSize(bucketCount, bucketLength); }

		[[nodiscard]] uint64& getBucketLength(const uint32 index) const { return *getKeysBucketPointer(index); }
		[[nodiscard]] uint64& getBucketLength(const uint32 index, const uint32 length) const { return *getKeysBucketPointer(index); }

		byte* allocate(const uint32 newCapacity, const uint32 newMaxBucketLength)
		{
			uint64 allocated_size{ 0 };	void* memory{ nullptr };
			allocator.Allocate(getTotalAllocationSize(newCapacity, newMaxBucketLength), alignof(T), &memory, &allocated_size);
			return static_cast<byte*>(memory);
		}

		void deallocate() const { allocator.Deallocate(getTotalAllocationSize(this->bucketCount, maxBucketLength), alignof(T), this->data); }

		void initializeBuckets() { for (uint32 i = 0; i < this->bucketCount; ++i) { getBucketLength(i) = 0; } }
		void initializeBuckets(const uint32 oldCapacity) { for (uint32 i = oldCapacity - 1; i < this->bucketCount; ++i) { getBucketLength(i) = 0; } }

		void copy(const uint32 newCapacity, const uint32 newMaxBucketLength, byte* to)
		{
			for (uint32 i = 0; i < this->bucketCount; ++i)
			{
				MemCopy(getKeysBucketAllocationSize(maxBucketLength), getKeysBucketPointer(i), getKeysBucketPointer(to, i, newMaxBucketLength));
				MemCopy(getValuesBucketAllocationsSize(maxBucketLength), getValuesBucketPointer(i), getValuesBucketPointer(to, i, newMaxBucketLength, newCapacity));
			}
		}

		[[nodiscard]] key_type* getKeysBucketPointer(const uint32 index) const { return getKeysBucketPointer(data, index, maxBucketLength); }
		[[nodiscard]] key_type* getKeysBucketPointer(const uint32 index, const uint32 buecketLength) const { return getKeysBucketPointer(data, index, buecketLength); }
		static key_type* getKeysBucketPointer(byte* to, const uint32 index, const uint32 bucketLength) { return reinterpret_cast<key_type*>(to) + index * (bucketLength + 1); }

		T* getValuesBucketPointer(const uint32 bucketIndex) const { return getValuesBucketPointer(data, bucketIndex, maxBucketLength, bucketCount); }
		T* getValuesBucketPointer(const uint32 bucketIndex, const uint32 bucketLength) const { return getValuesBucketPointer(data, bucketIndex, bucketLength, bucketCount); }
		static T* getValuesBucketPointer(byte* to, const uint32 bucketIndex, const uint32 bucketLength, const uint32 bucketCount) { return reinterpret_cast<T*>(to + getKeysAllocationSize(bucketCount, bucketLength)) + bucketIndex * bucketLength; }

		template<typename TT, class ALLOCATOR, typename L>
		friend void ForEach(FlatHashMap<TT, ALLOCATOR>& collection, L&& lambda);

		template<typename TT, class ALLOCATOR, typename L>
		friend void ForEach(const FlatHashMap<TT, ALLOCATOR>& collection, L&& lambda);

		template<typename TT, class ALLOCATOR, typename L>
		friend void PairForEach(FlatHashMap<TT, ALLOCATOR>& collection, L&& lambda);

		template<typename TT, class ALLOCATOR, typename L>
		friend void ForEachKey(FlatHashMap<TT, ALLOCATOR>& collection, L&& lambda);

		template<typename T, class ALLOCATOR1, class ALLOCATOR2>
		friend void Insert(const FlatHashMap<T, ALLOCATOR1>&, Buffer<ALLOCATOR2>& buffer);
		template<typename T, class ALLOCATOR1, class ALLOCATOR2>
		friend void Extract(FlatHashMap<T, ALLOCATOR1>&, Buffer<ALLOCATOR2>& buffer);
	};

	template<typename T, class ALLOCATOR, typename L>
	void ForEach(FlatHashMap<T, ALLOCATOR>& collection, L&& lambda)
	{
		for (uint32 bucketIndex = 0; bucketIndex < collection.bucketCount; ++bucketIndex) { for (auto& e : collection.getValuesBucket(bucketIndex)) { lambda(e); } }
	}

	template<typename T, class ALLOCATOR, typename L>
	void ForEach(const FlatHashMap<T, ALLOCATOR>& collection, L&& lambda)
	{
		for (uint32 bucketIndex = 0; bucketIndex < collection.bucketCount; ++bucketIndex) { for (const auto& e : collection.getValuesBucket(bucketIndex)) { lambda(e); } }
	}

	template<typename T, class ALLOCATOR, typename L>
	void ForEachKey(const FlatHashMap<T, ALLOCATOR>& collection, L&& lambda)
	{
		for (uint32 bucketIndex = 0; bucketIndex < collection.bucketCount; ++bucketIndex) { for (const auto e : collection.getKeysBucket(bucketIndex)) { lambda(e); } }
	}

	template<typename T, class ALLOCATOR, typename L>
	void PairForEach(FlatHashMap<T, ALLOCATOR>& collection, L&& lambda)
	{
		for (uint32 bucketIndex = 0; bucketIndex < collection.bucketCount; ++bucketIndex)
		{
			for (uint32 i = 0; i < collection.getKeysBucket(bucketIndex).ElementCount(); ++i)
			{
				lambda(collection.getKeysBucket(bucketIndex)[i], collection.getValuesBucket(bucketIndex)[i]);
			}
		}
	}
}