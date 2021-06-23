#pragma once

#include "GTSL/Core.h"
#include "GTSL/Array.hpp"
#include "Result.h"
#include "ArrayCommon.hpp"
#include "Range.h"

namespace GTSL
{
	template<typename K, typename V, uint32 X, uint32 Y>
	class StaticMap;
	
	template<typename K, typename V, uint32 X, uint32 Y>
	class StaticMapIterator {
	public:
		StaticMapIterator(StaticMap<K, V, X, Y>* m) : map(m) {
			++*this;
		}

		V& operator*();
		StaticMapIterator operator++();
		bool operator!=(const StaticMapIterator& other) const;
		bool operator!=(const bool other) const { return bucketElement != 0xFFFFFFFF; }

	private:
		StaticMap<K, V, X, Y>* map = nullptr;
		uint32 bucket = 0, bucketElement = 0xFFFFFFFF;
	};

	template<typename K, typename V, uint32 X, uint32 Y = X / 2>
	class StaticMap
	{
	public:
		StaticMap() = default;

		static_assert((X & X - 1) == 0, "X must be power of two!");

		auto begin() {
			return StaticMapIterator<K, V, X, Y>(this);
		}

		auto end() {
			return true;
		}
		
		template<typename... ARGS>
		V& Emplace(const K& key, ARGS&&... args)
		{
			auto bucketIndex = modulo(key);
			auto* res = ::new(&values[bucketIndex] + keys[bucketIndex].GetLength()) V(ForwardRef<ARGS>(args)...);
			keys[bucketIndex].EmplaceBack(static_cast<uint64>(key));
			return *res;
		}

		void Remove(const K& key)
		{
			auto bucketIndex = modulo(key); auto index = findElementIndex(keys[bucketIndex], key);
			popElement(Range<V*>(keys[bucketIndex].GetLength(), values[bucketIndex]), index.Get());
			keys[bucketIndex].Pop(index.Get());
		}

		V& At(const K& key)
		{
			auto bucketIndex = modulo(key); auto index = findElementIndex(keys[bucketIndex], key);
			GTSL_ASSERT(index.State(), "No entry by that name!");
			return values[bucketIndex][index.Get()];
		}

		const V& At(const K& key) const
		{
			auto bucketIndex = modulo(key); auto index = findElementIndex(keys[bucketIndex], key);
			GTSL_ASSERT(index.State(), "No entry by that name!");
			return values[bucketIndex][index.Get()];
		}

		[[nodiscard]] bool Find(const K& key) const { return findElementIndex(keys[modulo(key)], key).State(); }

	private:
		using keys_bucket = Array<uint64, Y>;
		keys_bucket keys[X];
		V values[X][Y];
		
		static Result<uint32> findElementIndex(const keys_bucket& keysBucket, const K& key)
		{
			uint32 i = 0;
			for (auto e : keysBucket) { if (e == static_cast<uint64>(key)) return Result<uint32>(MoveRef(i), true); ++i; }
			return Result<uint32>(false);
		}

		static uint32 modulo(const K& key) { return static_cast<uint64>(key) & (X - 1); }

		template<typename KK, typename VV, uint32 X, uint32 Y, typename L>
		friend void ForEach(StaticMap<KK, VV, X, Y>& staticMap, L&& lambda);

		friend StaticMapIterator<K, V, X, Y>;
	};

	template <typename K, typename V, uint32 X, uint32 Y>
	V& StaticMapIterator<K, V, X, Y>::operator*() { return map->values[bucket][bucketElement]; }

	template <typename K, typename V, uint32 X, uint32 Y>
	StaticMapIterator<K, V, X, Y> StaticMapIterator<K, V, X, Y>::operator++() {
		while (bucket < X) {
			if(++bucketElement < map->keys[bucket].GetLength()) {
				return *this;
			}

			++bucket;
			bucketElement = 0xFFFFFFFF;
		}
		
		return *this;
	}

	template <typename K, typename V, uint32 X, uint32 Y>
	bool StaticMapIterator<K, V, X, Y>::operator!=(const StaticMapIterator& other) const {
		return bucket != other.bucket || bucketElement != other.bucketElement;
	}

	template<typename K, typename V, uint32 X, uint32 Y, typename L>
	void ForEach(StaticMap<K, V, X, Y>& staticMap, L&& lambda)
	{
		for (uint32 x = 0; x < X; ++x)
		{
			for (uint32 y = 0; y < staticMap.keys[x].GetLength(); ++y) { lambda(staticMap.values[x][y]); }
		}
	}
}