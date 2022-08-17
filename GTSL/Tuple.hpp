#pragma once

#include "Core.h"
#include "TypeTraits.hpp"

namespace GTSL
{	
	template<typename... ARGS> class Tuple;

	template <> class Tuple<> {};

	template <class T>
	struct unwrap_refwrapper
	{
	    using type = T;
	};
	 
	template <class T>
	struct unwrap_refwrapper<std::reference_wrapper<T>>
	{
	    using type = T&;
	};
	 
	template <class T>
	using unwrap_decay_t = typename unwrap_refwrapper<typename std::decay<T>::type>::type;
	// or use std::unwrap_ref_decay_t (since C++20)

	template<typename T>
	class Tuple<T> {
	public:
		Tuple() = default;
		//Tuple(const T arg) : element(arg) {}
		//Tuple(const T& arg) : element(arg) {}
		Tuple(T&& arg) : element(GTSL::ForwardRef<T>(arg)) {}

		T element;
	};

	template<typename T, typename... TYPES>
	class Tuple<T, TYPES...> {
	public:
		Tuple() = default;
		//Tuple(const T arg, const TYPES... types) : element(arg), rest(types...) {}
		//Tuple(const T& arg, const TYPES&... types) : element(arg), rest(types...) {}
		Tuple(T&& arg, TYPES&&... types) : element(GTSL::ForwardRef<T>(arg)), rest(GTSL::ForwardRef<TYPES>(types)...) {}

		using type = T;
		
		T element;
		Tuple<TYPES...> rest;
	};

	template <class... Types>
	constexpr // since C++14
	auto make_tuple(Types&&... args) {
	    return Tuple<unwrap_decay_t<Types>...>(GTSL::ForwardRef<Types>(args)...);
	}

	template <class... Types>
	constexpr // since C++14
	auto ref_tuple(const Types&... args) {
	    return Tuple<const Types&...>(GTSL::ref(args)...);
	}

	template<uint64 N, typename T>
	struct TupleElement;

	template<uint64 N, typename T, typename... TYPES>
	struct TupleElement<N, Tuple<T, TYPES...>> : TupleElement<N - 1, Tuple<TYPES...>> {};

	template<typename T, typename... TYPES>
	struct TupleElement<0, Tuple<T, TYPES...>> { using type = T; };

	template <uint64 N>
	struct TupleAccessor {
		template <class... TYPES>
		static typename TupleElement<N, Tuple<TYPES...> >::type& Get(Tuple<TYPES...>& t) {
			return TupleAccessor<N - 1>::Get(t.rest);
		}
	
		template <class... TYPES>
		static const typename TupleElement<N, Tuple<TYPES...> >::type& Get(const Tuple<TYPES...>& t) {
			return TupleAccessor<N - 1>::Get(t.rest);
		}
	
		template <class... TYPES>
		static typename TupleElement<N, Tuple<TYPES...> >::type&& Get(Tuple<TYPES...>&& t) {
			return TupleAccessor<N - 1>::Get(t.rest);
		}
	
		template <class... TYPES>
		static const typename TupleElement<N, Tuple<TYPES...> >::type&& Get(const Tuple<TYPES...>&& t) {
			return TupleAccessor<N - 1>::Get(t.rest);
		}
	};
	
	template <>
	struct TupleAccessor<0> {
		template <class... Ts>
		static typename TupleElement<0, Tuple<Ts...> >::type& Get(Tuple<Ts...>& t) {
			return t.element;
		}
	
		template <class... Ts>
		static const typename TupleElement<0, Tuple<Ts...> >::type& Get(const Tuple<Ts...>& t) {
			return t.element;
		}
	
		template <class... Ts>
		static typename TupleElement<0, Tuple<Ts...> >::type&& Get(Tuple<Ts...>&& t) {
			return t.element;
		}
	
		template <class... Ts>
		static const typename TupleElement<0, Tuple<Ts...> >::type&& Get(const Tuple<Ts...>&& t) {
			return t.element;
		}
	};

	template<uint64...>
	struct Indices {};

	template<uint64 N, uint64... S>
	struct BuildIndices : BuildIndices<N - 1, N - 1, S...> {};

	template<uint64... S>
	struct BuildIndices<0, S...> : Indices<S...> {};

	template<class T, typename... TYPES>
	constexpr bool IsInTuple(Tuple<TYPES...>) noexcept {
		return IsInPack<T, TYPES...>();
	}

	template <class T, class Tuple>
	struct TypeIndexInTuple;

	template <class T, class... Types>
	struct TypeIndexInTuple<T, Tuple<T, Types...>> {
		static constexpr std::size_t value = 0;
	};

	template <class T, class U, class... Types>
	struct TypeIndexInTuple<T, Tuple<U, Types...>> {
		static constexpr std::size_t value = 1 + TypeIndexInTuple<T, Tuple<Types...>>::value;
	};

	template<uint64 N, typename... ARGS>
	auto& Get(Tuple<ARGS...>& tuple) { return TupleAccessor<N>::Get(tuple); }
	
	template<uint64 N, typename... ARGS>
	const auto& Get(const Tuple<ARGS...>& tuple) { return TupleAccessor<N>::Get(tuple); }
	
	template<uint64 N, typename... ARGS>
	auto&& Get(Tuple<ARGS...>&& tuple) { return TupleAccessor<N>::Get(tuple); }
	
	template<uint64 N, typename... ARGS>
	const auto&& Get(const Tuple<ARGS...>&& tuple) { return TupleAccessor<N>::Get(tuple); }

	template <class... ARGS>
	Tuple(ARGS...)->Tuple<ARGS...>;
}