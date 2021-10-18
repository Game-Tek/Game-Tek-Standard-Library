#pragma once

#include <concepts>

#include "Core.h"

#include "Memory.h"
#include "Range.hpp"
#include <new>

#include "Algorithm.h"
#include "Allocator.h"
#include "ArrayCommon.hpp"
#include "Bitman.h"
#include "Result.h"
#include "Serialize.hpp"

#if (_DEBUG)
#include "Assert.h"
#endif

namespace GTSL
{
	template<typename K, typename V, Allocator ALLOCATOR>
	class HashMap
	{
	public:
		HashMap(const ALLOCATOR& allocatorReference = ALLOCATOR()) : HashMap(2, allocatorReference)
		{
		}

		HashMap(const uint32 size, const ALLOCATOR& allocatorReference = ALLOCATOR()) : allocator(allocatorReference), bucketCount(NextPowerOfTwo(size)), bucketCapacity(bucketCount / 2)
		{
			while (bucketCapacity == 0) {
				bucketCount *= 2;
				bucketCapacity = bucketCount / 2;
			}
			
			data = allocate(bucketCount, bucketCapacity); initializeBuckets();
		}

		HashMap(const uint32 size, const float32 loadFactor, const ALLOCATOR& allocatorReference = ALLOCATOR()) : allocator(allocatorReference), bucketCount(NextPowerOfTwo(size)), bucketCapacity(static_cast<uint32>(bucketCount * loadFactor))
		{
			while (bucketCapacity == 0) {
				bucketCount *= 2;
				bucketCapacity = bucketCount / 2;
			}
			
			data = allocate(bucketCount, bucketCapacity); initializeBuckets();
		}

		HashMap(HashMap&& other) noexcept requires std::move_constructible<ALLOCATOR> : allocator(MoveRef(other.allocator)), data(other.data), bucketCount(other.bucketCount), bucketCapacity(other.bucketCapacity)
		{
			other.data = nullptr; other.bucketCount = 0;
		}

		void Free()
		{
			for (uint32 i = 0; i < this->bucketCount; ++i) {
				for (auto& e : this->getValuesBucket(i)) { Destroy(e); }
			}

			deallocate();
			this->data = nullptr;
		}
		
		~HashMap() {
			if (this->data) { Free(); }
		}

		template<typename TT>
		class Iterator
		{
		public:
			Iterator(HashMap* hashMap) : map(hashMap)
			{
				while (bucketIndex < map->bucketCount) {
					if (map->getKeysBucket(bucketIndex).ElementCount()) {
						break;
					}

					++bucketIndex;
				}
			}

			Iterator operator++()
			{
				while (bucketIndex < map->bucketCount) {
					if (++bucketElement < map->getKeysBucket(bucketIndex).ElementCount()) {
						return *this;
					}

					++bucketIndex;
					bucketElement = 0xFFFFFFFF;
				}

				return *this;
			}

			TT& operator*() const { return map->getValuesBucket(bucketIndex)[bucketElement]; }
			TT* operator->() const { return map->getValuesBucket(bucketIndex).begin() + bucketElement; }

			bool operator==(const Iterator& other) const { return bucketIndex == other.bucketIndex && bucketElement == other.bucketElement; }
			bool operator!=(const Iterator& other) const { return bucketIndex != other.bucketIndex || bucketElement != other.bucketElement; }
			bool operator!=(const bool) const { return bucketElement != 0xFFFFFFFF; }

		private:
			HashMap* map{ nullptr };
			uint32 bucketIndex = 0, bucketElement = 0;
		};

		Iterator<V> begin() { return Iterator<V>(this); }

		[[nodiscard]] bool end() const { return true; }

		Iterator<const V> begin() const { return Iterator<const V>(this); }

		template<typename... ARGS>
		V& Emplace(const K key, ARGS&&... args)
		{
			auto bucketIndex = ModuloByPowerOf2(static_cast<uint64>(key), this->bucketCount);
			GTSL_ASSERT(findKeyInBucket(bucketIndex, key) == nullptr, "Key already exists!")

			uint64 placeIndex = getBucketLength(bucketIndex);
			
			if (placeIndex + 1 > bucketCapacity) {
				resize();
				bucketIndex = ModuloByPowerOf2(static_cast<uint64>(key), this->bucketCount);
				placeIndex = getBucketLength(bucketIndex)++;
			} else {
				++getBucketLength(bucketIndex);
			}

			getKeysBucket(bucketIndex)[placeIndex] = static_cast<uint64>(key);
			return *new(getValuesBucket(bucketIndex).begin() + placeIndex) V(ForwardRef<ARGS>(args)...);
		}

		template<typename... ARGS>
		Result<V&> TryEmplace(const K key, ARGS&&... args)
		{
			auto result = TryGet(key);

			if(result.State()) {
				return GTSL::Result<V&>(result.Get(), false);
			}
			else {
				return GTSL::Result<V&>(Emplace(key, GTSL::ForwardRef<ARGS>(args)...), true);
			}
		}

		[[nodiscard]] bool Find(const K key) const { return findKeyInBucket(ModuloByPowerOf2(static_cast<uint64>(key), this->bucketCount), key); }

		[[nodiscard]] Result<V&> TryGet(const K key) {
			const auto bucket = ModuloByPowerOf2(static_cast<uint64>(key), this->bucketCount);
			auto result = getIndexForKeyInBucket(bucket, key);
			return GTSL::Result<V&>(*(getValuesBucketPointer(bucket) + result.Get()), result.State());
		}

		[[nodiscard]] Result<const V&> TryGet(const K key) const {
			const auto bucket = ModuloByPowerOf2(static_cast<uint64>(key), this->bucketCount);
			auto result = getIndexForKeyInBucket(bucket, key);
			return GTSL::Result<const V&>(*(getValuesBucketPointer(bucket) + result.Get()), result.State());
		}

		V& At(const K key) {
			const auto bucketIndex = ModuloByPowerOf2(static_cast<uint64>(key), bucketCount); const auto elementIndex = getIndexForKeyInBucket(bucketIndex, key);
			GTSL_ASSERT(elementIndex.State(), "No element with that key!");
			return getValuesBucket(bucketIndex)[elementIndex.Get()];
		}

		const V& At(const K key) const {
			const auto bucketIndex = ModuloByPowerOf2(static_cast<uint64>(key), bucketCount); const auto elementIndex = getIndexForKeyInBucket(bucketIndex, key);
			GTSL_ASSERT(elementIndex.State(), "No element with that key!");
			return getValuesBucket(bucketIndex)[elementIndex.Get()];
		}

		void Remove(const K key) {
			auto bucketIndex = ModuloByPowerOf2(static_cast<uint64>(key), this->bucketCount); auto elementIndex = getIndexForKeyInBucket(bucketIndex, key);
			Destroy(getValuesBucket(bucketIndex)[elementIndex.Get()]);
			GTSL_ASSERT(elementIndex.State(), "Key doesn't exist!")
			popElement(getKeysBucket(bucketIndex), elementIndex.Get());
			popElement(getValuesBucket(bucketIndex), elementIndex.Get());
			--getBucketLength(bucketIndex);
		}

		void Clear()
		{
			for (uint32 bucket = 0; bucket < this->bucketCount; ++bucket) {
				for (auto& e : getValuesBucket(bucket)) {
					Destroy(e);
				}

				getBucketLength(bucket) = 0;
			}
		}

		V& operator[](const K key) { return At(key); }
		const V& operator[](const K key) const { return At(key); }
	
	private:
		friend class Iterator<V>;

		[[no_unique_address]] ALLOCATOR allocator;
		byte* data = nullptr;
		uint32 bucketCount = 0;
		uint32 bucketCapacity = 0;

		[[nodiscard]] uint64* findKeyInBucket(const uint32 keys_bucket, const K key) const
		{
			//const auto simd_elements = modulo(getBucketLength(bucket), this->capacity);
			//
			//SIMD<K> key_vector(key);
			//
			//for (uint32 i = 0; i < simd_elements; ++i)
			//{
			//	SIMD<K> keys(UnalignedPointer<uint64>(getKeysBucket(bucket)));
			//	auto res = keys == key_vector;
			//	auto bb = res.GetElement<0>();
			//	if(bb & key || cc & key)
			//}

			for (auto& e : getKeysBucket(keys_bucket)) { if (e == static_cast<uint64>(key)) { return &e; } } return nullptr;
		}

		[[nodiscard]] Result<uint32> getIndexForKeyInBucket(const uint32 bucketIndex, const K& key) const
		{
			for (auto& e : getKeysBucket(bucketIndex)) { if (e == static_cast<uint64>(key)) { return GTSL::Result(static_cast<uint32>(&e - getKeysBucket(bucketIndex).begin()), true); } }
			return GTSL::Result<uint32>(0, false);
		}

		[[nodiscard]] Range<uint64*> getKeysBucket(const uint32 bucketIndex) const { return Range<uint64*>(getBucketLength(bucketIndex), getKeysBucketPointer(bucketIndex) + 1); }
		[[nodiscard]] Range<uint64*> getKeysBucket(const uint32 bucketIndex, const uint32 length) const { return Range<uint64*>(getBucketLength(bucketIndex, length), getKeysBucketPointer(bucketIndex, length) + 1); }

		[[nodiscard]] Range<V*> getValuesBucket(const uint32 bucketIndex) const { return Range<V*>(getBucketLength(bucketIndex), getValuesBucketPointer(bucketIndex)); }
		[[nodiscard]] Range<V*> getValuesBucket(const uint32 bucketIndex, const uint32 length) const { return Range<V*>(getBucketLength(bucketIndex, length), getValuesBucketPointer(bucketIndex, length)); }

		void resize()
		{
			auto newBucketCount = bucketCount * 2;
			auto newBucketCapacity = bucketCapacity * 2u;
			auto* newAlloc = allocate(newBucketCount, newBucketCapacity);

			for (uint32 bucketIndex = 0; bucketIndex < newBucketCount; ++bucketIndex) {
				*getKeysBucketLength(newAlloc, bucketIndex, newBucketCapacity) = 0;
			}			

			for(uint32 bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex) {
				for(uint32 bucketElementIndex = 0; bucketElementIndex < getBucketLength(bucketIndex); ++bucketElementIndex) {
					auto newBucketIndex = ModuloByPowerOf2(getKeysBucket(bucketIndex)[bucketElementIndex], newBucketCount);
					auto& newBucketLength = *getKeysBucketLength(newAlloc, newBucketIndex, newBucketCapacity);
					getKeysBucket(newAlloc, newBucketIndex, newBucketCapacity)[newBucketLength] = getKeysBucket(bucketIndex)[bucketElementIndex];
					MemCopy(sizeof(V), getValuesBucketPointer(data, bucketIndex, bucketCapacity, bucketCount) + bucketElementIndex, getValuesBucketPointer(newAlloc, newBucketIndex, newBucketCapacity, newBucketCount) + newBucketLength);
					++newBucketLength;
				}
			}

			deallocate();
			data = newAlloc; bucketCount = newBucketCount; bucketCapacity = newBucketCapacity;
		}

		static uint64 getKeysBucketAllocationSize(const uint32 bucketLength) { return (bucketLength + 1) * sizeof(uint64); }
		static uint64 getKeysAllocationSize(const uint32 bucketCount, const uint32 maxBucketLength) { return getKeysBucketAllocationSize(maxBucketLength) * bucketCount; }

		static uint64 getValuesBucketAllocationsSize(const uint32 bucketLength) { return bucketLength * sizeof(V); }
		static uint64 getValuesAllocationsSize(const uint32 bucketCount, const uint32 maxBucketLength) { return getValuesBucketAllocationsSize(maxBucketLength) * bucketCount; }

		static uint64 getTotalAllocationSize(const uint32 bucketCount, const uint32 bucketLength) { return getKeysAllocationSize(bucketCount, bucketLength) + getValuesAllocationsSize(bucketCount, bucketLength); }

		[[nodiscard]] uint64& getBucketLength(const uint32 index) const { return *getKeysBucketPointer(index); }
		[[nodiscard]] uint64& getBucketLength(const uint32 index, const uint32 length) const { return *getKeysBucketPointer(index); }

		byte* allocate(const uint32 newCapacity, const uint32 newMaxBucketLength)
		{
			uint64 allocated_size{ 0 };	void* memory{ nullptr };
			allocator.Allocate(getTotalAllocationSize(newCapacity, newMaxBucketLength), alignof(V), &memory, &allocated_size);
			return static_cast<byte*>(memory);
		}

		void deallocate() { allocator.Deallocate(getTotalAllocationSize(this->bucketCount, bucketCapacity), alignof(V), this->data); }

		void initializeBuckets() { for (uint32 i = 0; i < this->bucketCount; ++i) { getBucketLength(i) = 0; } }

		[[nodiscard]] uint64* getKeysBucketPointer(const uint32 index) const { return getKeysBucketLength(data, index, bucketCapacity); }
		[[nodiscard]] uint64* getKeysBucketPointer(const uint32 index, const uint32 buecketLength) const { return getKeysBucketLength(data, index, buecketLength); }

		static uint64* getKeysBucketLength(byte* to, const uint32 index, const uint32 bucketCapacity) { return reinterpret_cast<uint64*>(to) + index * (bucketCapacity + 1); }
		static uint64* getKeysBucket(byte* to, const uint32 index, const uint32 bucketCapacity) { return reinterpret_cast<uint64*>(to) + index * (bucketCapacity + 1) + 1; }

		V* getValuesBucketPointer(const uint32 bucketIndex) const { return getValuesBucketPointer(data, bucketIndex, bucketCapacity, bucketCount); }
		V* getValuesBucketPointer(const uint32 bucketIndex, const uint32 bucketLength) const { return getValuesBucketPointer(data, bucketIndex, bucketLength, bucketCount); }

		static V* getValuesBucketPointer(byte* to, const uint32 bucketIndex, const uint32 bucketCapacity, const uint32 bucketCount) { return reinterpret_cast<V*>(to + getKeysAllocationSize(bucketCount, bucketCapacity)) + bucketIndex * bucketCapacity; }

		template<typename KK, typename VV, class ALLOCATOR, typename L>
		friend void ForEach(HashMap<KK, VV, ALLOCATOR>& collection, L&& lambda);

		template<typename KK, typename VV, class ALLOCATOR, typename L>
		friend void ForEach(const HashMap<KK, VV, ALLOCATOR>& collection, L&& lambda);

		template<typename KK, typename VV, class ALLOCATOR, typename L>
		friend void PairForEach(HashMap<KK, VV, ALLOCATOR>& collection, L&& lambda);

		template<typename KK, typename VV, class ALLOCATOR, typename L>
		friend void ForEachKey(HashMap<KK, VV, ALLOCATOR>& collection, L&& lambda);

	public:
		friend void Insert(const HashMap& map, auto& buffer) {
			Insert(map.bucketCount, buffer);
			Insert(map.bucketCapacity, buffer);
		
			for (uint32 bucket = 0; bucket < map.bucketCount; ++bucket) {
				Insert(map.getBucketLength(bucket), buffer);
				auto keysBucket = map.getKeysBucket(bucket);
				for (auto e : keysBucket) { Insert(e, buffer); }
				auto valuesBucket = map.getValuesBucket(bucket);
				for (auto& e : valuesBucket) { Insert(e, buffer); }
			}
		}

		friend void Extract(HashMap& map, auto& buffer) {
			uint32 capacity{ 0 }, maxBucketLength = 0;
			Extract(capacity, buffer);
			Extract(maxBucketLength, buffer);
		
			for (uint32 bucket = 0; bucket < capacity; ++bucket) {
				Extract(map.getBucketLength(bucket), buffer);
				auto keysBucket = map.getKeysBucket(bucket);
				for (auto& e : keysBucket) { Extract(e, buffer); }
				auto valuesBucket = map.getValuesBucket(bucket);
				for (auto& e : valuesBucket) { Extract(e, buffer); }
			}
		}
	};
	
	template<typename K, typename V, uint64 N>
	using StaticMap = HashMap<K, V, StaticAllocator<N * (sizeof(uint64) * (N + 1)) + N * sizeof(V) * N>>;
	
	template<typename K, typename V, class ALLOCATOR, typename L>
	void ForEach(HashMap<K, V, ALLOCATOR>& collection, L&& lambda) {
		for (uint32 bucketIndex = 0; bucketIndex < collection.bucketCount; ++bucketIndex) { for (auto& e : collection.getValuesBucket(bucketIndex)) { lambda(e); } }
	}

	template<typename K, typename V, class ALLOCATOR, typename L>
	void ForEach(const HashMap<K, V, ALLOCATOR>& collection, L&& lambda)
	{
		for (uint32 bucketIndex = 0; bucketIndex < collection.bucketCount; ++bucketIndex) { for (const auto& e : collection.getValuesBucket(bucketIndex)) { lambda(e); } }
	}

	template<typename K, typename V, class ALLOCATOR, typename L>
	void ForEachKey(const HashMap<K, V, ALLOCATOR>& collection, L&& lambda)
	{
		for (uint32 bucketIndex = 0; bucketIndex < collection.bucketCount; ++bucketIndex) { for (const auto e : collection.getKeysBucket(bucketIndex)) { lambda(e); } }
	}

	template<typename K, typename V, class ALLOCATOR, typename L>
	void PairForEach(HashMap<K, V, ALLOCATOR>& collection, L&& lambda)
	{
		for (uint32 bucketIndex = 0; bucketIndex < collection.bucketCount; ++bucketIndex)
		{
			for (uint32 i = 0; i < collection.getKeysBucket(bucketIndex).ElementCount(); ++i)
			{
				lambda(collection.getKeysBucket(bucketIndex)[i], collection.getValuesBucket(bucketIndex)[i]);
			}
		}
	}

	template<typename K, class ALLOCATOR>
	class HashMap<K, void, ALLOCATOR>
	{
	public:
		HashMap(const ALLOCATOR& allocator = ALLOCATOR()) : allocator(allocator) {}
		HashMap(const uint64 size, const ALLOCATOR& allocator = ALLOCATOR()) : allocator(allocator) {
			bucketCount = NextPowerOfTwo(size);
			bucketCapacity = 4;
			keys = allocate(bucketCount, bucketCapacity);
			initializeBuckets(keys, bucketCount, bucketCapacity);
		}

		~HashMap() {
			if(keys) {
				deallocate();
			}
		}

		void Emplace(K key) {
			auto bucketIndex = ModuloByPowerOf2(static_cast<uint64>(key), bucketCount);
			auto bucketLength = getBucketLength(bucketIndex, keys, bucketCapacity);

			if (bucketLength + 1 > bucketCapacity) {
				resize();
				bucketIndex = ModuloByPowerOf2(static_cast<uint64>(key), bucketCount);
				bucketLength = getBucketLength(bucketIndex, keys, bucketCapacity);
			}

			bucketEntry(bucketIndex, bucketLength, keys, bucketCapacity) = static_cast<uint64>(key);
			++getBucketLength(bucketIndex, keys, bucketCapacity);
		}

		[[nodiscard]] bool Find(const K key) const {
			auto bucketIndex = ModuloByPowerOf2(static_cast<uint64>(key), bucketCount);
			for(uint64 i = 0; i < getBucketLength(bucketIndex, keys, bucketCapacity); ++i) {
				if(bucketEntry(bucketIndex, i, keys, bucketCapacity) == static_cast<uint64>(key)) {
					return true;
				}
			}
			return false;
		}

	private:
		ALLOCATOR allocator;
		uint64* keys = nullptr;

		uint32 bucketCount = 0, bucketCapacity = 0;

		uint64* allocate(const uint32 newBucketCount, const uint32 newBucketSize) {
			uint64 allocated_size{ 0 };	void* memory{ nullptr };
			allocator.Allocate(newBucketCount * (newBucketSize + 1) * sizeof(uint64), alignof(uint64), &memory, &allocated_size);
			auto* data = static_cast<uint64*>(memory);
			return static_cast<uint64*>(memory);
		}

		void deallocate() {
			allocator.Deallocate(bucketCount * (bucketCapacity + 1) * sizeof(uint64), alignof(uint64), keys);
			keys = nullptr;
		}

		static uint64& getBucketLength(uint32 bucketIndex, uint64* keys, uint32 bucketCapacity) {
			return keys[bucketIndex * (bucketCapacity + 1)];
		}

		static uint64& bucketEntry(uint32 bucketIndex, uint32 element, uint64* keys, uint32 bucketCapacity) {
			return keys[bucketIndex * (bucketCapacity + 1) + element + 1];
		}

		static void initializeBuckets(uint64* buckets, uint32 bucketCount, uint32 bucketCapacity) {
			for (uint32 bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex) {
				getBucketLength(bucketIndex, buckets, bucketCapacity) = 0;
			}
		}

		void resize() {
			auto newBucketCount = bucketCount * 2;
			auto newBucketCapacity = bucketCapacity * 2u;
			auto* newAlloc = allocate(newBucketCount, newBucketCapacity);

			initializeBuckets(newAlloc, newBucketCount, newBucketCapacity);

			for (uint32 bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex) {
				for (uint32 bucketElementIndex = 0; bucketElementIndex < getBucketLength(bucketIndex, keys, bucketCapacity); ++bucketElementIndex) {
					auto newBucketIndex = ModuloByPowerOf2(bucketEntry(bucketIndex, bucketElementIndex, keys, bucketCapacity), newBucketCount);
					auto& newBucketLength = getBucketLength(newBucketIndex, newAlloc, newBucketCapacity);
					bucketEntry(newBucketIndex, newBucketLength, newAlloc, newBucketCapacity) = bucketEntry(bucketIndex, bucketElementIndex, keys, bucketCapacity);
					++newBucketLength;
				}
			}

			deallocate();
			keys = newAlloc; bucketCount = newBucketCount; bucketCapacity = newBucketCapacity;
		}
	};

	template<typename K, class ALLOCATOR>
	using KeyMap = HashMap<K, void, ALLOCATOR>;
}