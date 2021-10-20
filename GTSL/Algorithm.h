#pragma once

#include "Result.h"
#include "Tuple.h"
#include "Range.hpp"

namespace GTSL
{
	
	template <typename... ARGS, uint64... IS>
	auto Call(auto&& lambda, Tuple<ARGS...>&& tup, Indices<IS...>) { return lambda(GTSL::MoveRef(TupleAccessor<IS>::Get(tup))...); }
	
	template <typename... ARGS>
	auto Call(auto&& lambda, Tuple<ARGS...>&& tup) { return Call(lambda, GTSL::MoveRef(tup), BuildIndices<sizeof...(ARGS)>{}); }

	constexpr uint32 ModuloByPowerOf2(const uint64 key, const uint32 size) { return key & (size - 1); }

	template<typename T>
	void Destroy(T& object) { object.~T(); }
	
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

	template<typename... ARGS>
	consteval size_t PackSize() {
		return (sizeof(ARGS) + ... + 0);
	}

	template <int N, typename... T>
	struct TypeAt;

	template <typename T0, typename... T>
	struct TypeAt<0, T0, T...> {
		typedef T0 type;
	};
	template <int N, typename T0, typename... T>
	struct TypeAt<N, T0, T...> {
		typedef typename TypeAt<N - 1, T...>::type type;
	};

	template<uint32 AT, uint32 I, typename... ARGS>
	consteval size_t ttt() {
		if constexpr (AT == I)
			return 0;
		else
			return sizeof(TypeAt<I, ARGS...>) + ttt<AT, I + 1, ARGS...>();
	}

	template<uint32 AT, typename... ARGS>
	consteval size_t PackSizeAt() {
		return ttt<AT, 0, ARGS...>();
	}

	template<typename F, typename... ARGS>
	void MultiFor(F&& function, uint32 length, ARGS&&... iterators)
	{
		auto add = [](auto& n) { ++n; };
		
		for (uint32 i = 0; i < length; ++i) {
			function(i, *(iterators.begin())...);
			add(iterators.begin()...);
		}
	}

	constexpr auto LookFor(auto& iterable, auto&& function) {
		for (auto begin = iterable.begin(); begin != iterable.end(); ++begin) {
			if (function(*begin)) {
				return Result(MoveRef(begin), true);
			}
		}
		
		return Result<decltype(iterable.begin())>(false);
	}

	template<typename A, typename B, typename C>
	bool CrossSearch(const A& range0, const B& range1, const C& obj) {
		for (auto& a : range0) {
			if (a == obj) {
				for (auto& b : range1) {
					if (b == obj) {
						return true;
					}
				}
			}
		}

		return false;
	}

	template<typename A, typename B, typename C>
	void CrossSearch(const A& range0, const B& range1, const C& obj, auto&& f) {
		uint32 ia = 0, ib = 0;

		for (auto& a : range0) {
			if (a == obj) {
				for (auto& b : range1) {
					if (b == obj) {
						f(true, ia, ib);
						return;
					}

					++ib;
				}
			}

			++ia;
		}

		f(false, ia, ib);
	}

	template<typename A>
	void Sort(A& container, auto pred) {
		bool sorted = false;

		while (!sorted) {
			sorted = true;

			for (uint32 i = 0; i < container.GetLength() - 1; ++i) {
				if (pred(container[i], container[i + 1])) {
					auto aux = container[i];
					container[i] = container[i + 1];
					container[i + 1] = aux;

					sorted = false;
				}
			}
		}
	}

	template<typename A>
	void SortG(A& container) {
		Sort(container, [](const typename A::value_type& a, const typename A::value_type& b) -> bool { return a > b; });
	}

	template<typename A>
	void SortL(A& container) {
		Sort(container, [](const typename A::value_type& a, const typename A::value_type& b) -> bool { return a < b; });
	}
}
