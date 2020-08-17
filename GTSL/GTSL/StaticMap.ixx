module;

#include "GTSL/Core.h"
#include "GTSL/Array.hpp"
#include "Result.h"

export module StaticMap;

import ArrayCommon;

namespace GTSL
{
	export template<typename T, uint32 X, uint32 Y = X / 2>
	class StaticMap
	{
	public:
		StaticMap() = default;

		static_assert((X& (X - 1)) == 0, "X must be power of two!");

		template<typename... ARGS>
		void Emplace(const uint64 key, ARGS&&... args)
		{
			auto keys_bucket = modulo(key);
			::new(&values[keys_bucket] + keys[keys_bucket].GetLength()) T(ForwardRef<ARGS>(args)...);
			keys[keys_bucket].EmplaceBack(key);
		}

		void Remove(const uint64 key)
		{
			auto bucket = modulo(key); auto index = findElementIndex(keys[bucket], key);
			popElement(Ranger<T>(keys[bucket].GetLength(), values[bucket]), index);
			keys[bucket].Pop(index);
		}

		T& At(uint64 key)
		{
			auto bucket = modulo(key); auto index = findElementIndex(keys[bucket], key);
			GTSL_ASSERT(index, "No entry by that name!");
			return values[bucket][index];
		}

		bool Find(const uint64 key) const { return findElementIndex(keys[modulo(key)], key); }

	private:
		using keys_bucket = Array<uint64, Y>;
		keys_bucket keys[X];
		T values[X][Y];

		static Result<uint32> findElementIndex(const keys_bucket& keys_bucket, const uint64 key)
		{
			uint32 i = 0;
			for (auto e : keys_bucket) { if (e == key) return Result<uint32>(i, true); ++i; }
			return Result<uint32>(false);
		}

		static uint32 modulo(const uint64 key) { return key & (X - 1); }
	};
}