#pragma once

#include "Core.h"
#include "Result.h"
#include <type_traits>
#include <new>

namespace GTSL
{
	constexpr uint32 ModuloByPowerOf2(const uint64 key, const uint32 size) { return key & (size - 1); }

	template<typename T>
	void Destroy(T& object) { object.~T(); }
	
	template<typename T>
	constexpr uint64 Bits() { return sizeof(T) * 8; }
	
	template<typename T>
	bool IsPowerOfTwo(T number) { return (number & (number - 1)) == 0; }

	template<typename T>
	constexpr bool IsIntegral() { return false; };

	template<typename T>
	void Move(T* from, T* to) {
		::new(to) T(static_cast<T&&>(*from));
		Destroy(*from);
	}

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

	template<class T, class... Ts>
	constexpr bool IsInPack() noexcept {
		return (std::is_same_v<T, Ts> || ...);
	}

	template<size_t POS, typename... TYPES>
	struct TypeAt;

	template<typename HEAD, typename... TAIL >
	struct TypeAt<0, HEAD, TAIL...> {
		using type = HEAD;
	};

	template<size_t POS, typename HEAD, typename... TAIL >
	struct TypeAt<POS, HEAD, TAIL...> {
		using type = typename TypeAt<POS - 1, TAIL...>::type;
	};

	template<typename A, typename B>
	constexpr bool IsSame() { return std::is_same_v<A, B>; }

	template<typename... ARGS>
	consteval size_t PackSize() {
		return (sizeof(ARGS) + ... + 0);
	}

	template<uint32 I, typename Target, typename ListHead, typename... ListTails>
	constexpr uint32 GetTypeIndex_impl() {
		if constexpr (std::is_same_v<Target, ListHead>)
			return I;
		else
			return GetTypeIndex_impl<I + 1, Target, ListTails...>();
	}

	template<typename Target, typename... ListTails>
	constexpr uint32 GetTypeIndex() {
		static_assert(IsInPack<Target, ListTails...>(), "Type T is not in template pack.");
		return GetTypeIndex_impl<0, Target, ListTails...>();
	}

	template<uint32 AT, uint32 I, typename HEAD, typename... TAIL>
	consteval size_t PackSizeAt_impl() {
		if constexpr (AT == I)
			return 0;
		else
			return sizeof(HEAD) + PackSizeAt_impl<AT, I + 1, TAIL...>();
	}

	template<uint32 AT, typename... ARGS>
	consteval size_t PackSizeAt() {
		return PackSizeAt_impl<AT, 0, ARGS...>();
	}

	template<typename F, typename... ARGS>
	void MultiFor(F&& function, uint32 length, ARGS&&... iterators) {
		auto add = [](auto& n) { ++n; };
		
		for (uint32 i = 0; i < length; ++i) {
			function(i, *(iterators.begin())...);
			add(iterators.begin()...);
		}
	}

	template<class C>
	constexpr auto Find(C& iterable, auto&& function) -> Result<typename C::iterator> {
		for (auto begin = iterable.begin(); begin != iterable.end(); ++begin) {
			if (function(*begin)) {
				return Result(MoveRef(begin), true);
			}
		}
		
		return Result<typename C::iterator>(false);
	}

	template<class C>
	constexpr auto Find(const C& iterable, auto&& function) -> Result<typename C::const_iterator> {
		for (auto begin = iterable.begin(); begin != iterable.end(); ++begin) {
			if (function(*begin)) {
				return Result(MoveRef(begin), true);
			}
		}

		return Result<typename C::const_iterator>(false);
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

	template<typename T>
	void Max(T* var, T val) {
		if(val > *var) {
			*var = val;
		}
	}

	template<typename T>
	void Min(T* var, T val) {
		if (val < *var) {
			*var = val;
		}
	}
}
