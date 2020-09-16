#pragma once

#include "GTSL/Core.h"
#include "GTSL/Array.hpp"
#include "Result.h"
#include "ArrayCommon.hpp"

namespace GTSL
{
	template<typename T, uint32 X, uint32 Y = X / 2>
	class StaticMap
	{
	public:
		StaticMap() = default;

		static_assert((X & (X - 1)) == 0, "X must be power of two!");

		template<typename... ARGS>
		void Emplace(const uint64 key, ARGS&&... args)
		{
			auto bucketIndex = modulo(key);
			::new(&values[bucketIndex] + keys[bucketIndex].GetLength()) T(ForwardRef<ARGS>(args)...);
			keys[bucketIndex].EmplaceBack(key);
		}

		void Remove(const uint64 key)
		{
			auto bucketIndex = modulo(key); auto index = findElementIndex(keys[bucketIndex], key);
			popElement(Ranger<T>(keys[bucketIndex].GetLength(), values[bucketIndex]), index.Get());
			keys[bucketIndex].Pop(index.Get());
		}

		T& At(const uint64 key)
		{
			auto bucketIndex = modulo(key); auto index = findElementIndex(keys[bucketIndex], key);
			GTSL_ASSERT(index.State(), "No entry by that name!");
			return values[bucketIndex][index.Get()];
		}

		const T& At(const uint64 key) const
		{
			auto bucketIndex = modulo(key); auto index = findElementIndex(keys[bucketIndex], key);
			GTSL_ASSERT(index.State(), "No entry by that name!");
			return values[bucketIndex][index.Get()];
		}

		[[nodiscard]] bool Find(const uint64 key) const { return findElementIndex(keys[modulo(key)], key); }

	private:
		using keys_bucket = Array<uint64, Y>;
		keys_bucket keys[X];
		T values[X][Y];
		
		static Result<uint32> findElementIndex(const keys_bucket& keysBucket, const uint64 key)
		{
			uint32 i = 0;
			for (auto e : keysBucket) { if (e == key) return Result<uint32>(MoveRef(i), true); ++i; }
			return Result<uint32>(false);
		}

		static uint32 modulo(const uint64 key) { return key & (X - 1); }

		template<typename T, uint32 X, uint32 Y, typename L>
		friend void ForEach(StaticMap<T, X, Y>& staticMap, L&& lambda);
	};

	template<typename T, uint32 X, uint32 Y, typename L>
	void ForEach(StaticMap<T, X, Y>& staticMap, L&& lambda)
	{
		for (uint32 x = 0; x < X; ++x)
		{
			for (uint32 y = 0; y < staticMap.keys[x].GetLength(); ++y) { lambda(staticMap.values[x][y]); }
		}
	}
}