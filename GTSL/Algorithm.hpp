#pragma once

#include "Core.h"
#include "Result.h"
#include <new>
#include "Tuple.hpp"

namespace GTSL
{
	constexpr uint32 ModuloByPowerOf2(const uint64 key, const uint32 size) { return key & (size - 1); }

	template<typename T> struct Hash;

	template<typename T>
	void Destroy(T& object) { object.~T(); }
	
	template<typename T>
	constexpr uint64 Bits() { return sizeof(T) * 8; }
	
	template<typename T>
	bool IsPowerOfTwo(T number) { return (number & (number - 1)) == 0; }

	template<typename T>
	void Move(T* from, T* to) {
		::new(to) T(static_cast<T&&>(*from));
		Destroy(*from);
	}

	template<typename F, typename... ARGS>
	void MultiFor(F&& function, uint32 length, ARGS&&... iterators) {
		auto add = [](auto& n) { ++n; };
		
		for (uint32 i = 0; i < length; ++i) {
			function(i, *(iterators.begin())...);
			add(iterators.begin()...);
		}
	}

	template<typename... ARGS>
	uint64 GetTypeSize(uint32 index) {
		static constexpr uint64 sizes[sizeof...(ARGS)]{ sizeof(ARGS)... };
		return sizes[index];
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

	template<typename T>
	bool Contains(const auto& collection, const T& value) {
		for (const auto& e : collection) { if (e == value) { return true; } }
		return false;
	}

	template <typename F, typename... ARGS>
	auto CallForEach(F&& lambda, Tuple<ARGS...> tup) {
		return []<uint64... I>(F&& lambda, Tuple<ARGS...> tup, Indices<I...>) {
			return (lambda(Get<I>(tup)) && ...);
		}(ForwardRef<F>(lambda), tup, BuildIndices<sizeof...(ARGS)>{});
	}

	template<typename T, typename... ARGS>
	bool Contain(GTSL::Tuple<ARGS...> collections, const T& value) {
		auto s = [&](const auto& c) {
			return Contains(c, value);
		};

		return CallForEach(s, collections);
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
	void SortAscending(A& container) {
		Sort(container, [](const typename A::value_type& a, const typename A::value_type& b) -> bool { return a > b; });
	}

	template<typename A>
	void SortDescending(A& container) {
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
