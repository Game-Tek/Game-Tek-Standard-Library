#pragma once

#include "Tuple.h"

namespace GTSL
{	
	//template<typename CALLABLE, typename... ARGS>
	//static auto Call(CALLABLE&& callable, Tuple<ARGS...>& tup) { using indices = BuildIndices<sizeof...(ARGS)>; return callable(TupleAccessor<indices>::Get(tup)...); }
	
	template <typename LAMBDA, typename... ARGS, uint64... IS>
	static auto Call(LAMBDA&& lambda, Tuple<ARGS...>&& tup, Indices<IS...>) { return lambda(GTSL::MoveRef(TupleAccessor<IS>::Get(tup))...); }

	template <typename LAMBDA, typename... ARGS>
	static auto Call(LAMBDA&& lambda, Tuple<ARGS...>&& tup) { return Call(lambda, GTSL::MoveRef(tup), BuildIndices<sizeof...(ARGS)>{}); }

	template<typename T>
	constexpr uint64 Bits() { return sizeof(T) * 8; }
	
	template<typename T>
	bool IsPowerOfTwo(T number) { return (number & (number - 1)) == 0; }

	template<typename T>
	constexpr bool IsIntegral() { return false; };

	template<>
	constexpr bool IsIntegral<uint32>() { return true; }

	template<>
	constexpr bool IsIntegral<uint64>() { return true; }

	template<typename T>
	concept Integral = IsIntegral<T>();
	
	template<typename E>
	inline constexpr bool IsEnum = __is_enum(E);

	template<typename U>
	inline constexpr bool IsUnion = __is_union(U);

	template<typename T>
	inline constexpr bool IsPOD = __is_pod(T);
	
	template<typename E>
	concept Enum = IsEnum<E>;

	template<typename U>
	concept Union = IsUnion<U>;

	template<typename T>
	concept POD = IsPOD<T>;

	template<Enum E>
	using UnderlyingType = __underlying_type(E);
}
