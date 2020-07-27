#pragma once

#include "Delegate.hpp"
#include "Tuple.h"

namespace GTSL
{
	template <typename RET, typename T, typename... ARGS, uint64... IS>
	static auto Call(const Delegate<T>& delegate, Tuple<ARGS...>& tup, Indices<IS...>) -> RET
	{
		return delegate(TupleAccessor<IS>::Get(tup)...);
	}

	template <typename RET, typename... ARGS>
	static auto Call(const Delegate<RET(ARGS...)>& delegate, Tuple<ARGS...>& tup) -> RET
	{
		return Call<RET>(delegate, tup, BuildIndices<sizeof...(ARGS)>{});
	}

	template<typename T>
	bool IsPowerOfTwo(T number) { return (number & (number - 1)) == 0; }
}
