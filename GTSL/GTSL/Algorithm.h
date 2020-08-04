#pragma once

#include "Tuple.h"
#include "Delegate.hpp"

namespace GTSL
{
	//template<typename CALLABLE, typename... ARGS>
	//static auto Call(CALLABLE&& callable, Tuple<ARGS...>& tup) { using indices = BuildIndices<sizeof...(ARGS)>; return callable(TupleAccessor<indices>::Get(tup)...); }
	
	template <typename LAMBDA, typename... ARGS, uint64... IS>
	static auto Call(LAMBDA&& lambda, Tuple<ARGS...>& tup, Indices<IS...>) { return lambda(TupleAccessor<IS>::Get(tup)...); }

	template <typename LAMBDA, typename... ARGS>
	static auto Call(LAMBDA&& lambda, Tuple<ARGS...>& tup) { return Call(lambda, tup, BuildIndices<sizeof...(ARGS)>{}); }
	
	template<typename T>
	bool IsPowerOfTwo(T number) { return (number & (number - 1)) == 0; }

	template<typename E>
	using UnderlyingType = __underlying_type(E);
}
