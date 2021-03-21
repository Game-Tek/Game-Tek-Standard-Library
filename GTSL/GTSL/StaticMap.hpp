#pragma once

#include "GTSL/Core.h"
#include "GTSL/Array.hpp"
#include "Result.h"
#include "ArrayCommon.hpp"
#include "Range.h"

namespace GTSL
{
	template<typename K, typename V, uint32 X, uint32 Y = X / 2>
	class StaticMap
	{
	public:
		StaticMap() = default;

		static_assert((X & (X - 1)) == 0, "X must be power of two!");

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
	};

	template<typename K, typename V, uint32 X, uint32 Y, typename L>
	void ForEach(StaticMap<K, V, X, Y>& staticMap, L&& lambda)
	{
		for (uint32 x = 0; x < X; ++x)
		{
			for (uint32 y = 0; y < staticMap.keys[x].GetLength(); ++y) { lambda(staticMap.values[x][y]); }
		}
	}
}