#pragma once

#include "Core.h"

namespace GTSL
{	
	template<typename... ARGS> class Tuple;

	template <> class Tuple<> {};

	template<typename T>
	class Tuple<T>
	{
	public:
		Tuple() = default;
		Tuple(T&& arg) : element(GTSL::ForwardRef<T>(arg))
		{}

		T element;
	};

	template<typename T, typename... TYPES>
	class Tuple<T, TYPES...>
	{
	public:
		Tuple() = default;
		Tuple(T&& arg, TYPES&&... types) : element(GTSL::ForwardRef<T>(arg)), rest(GTSL::ForwardRef<TYPES>(types)...)
		{}

		T element;
		Tuple<TYPES...> rest;
	};

	template<uint64 N, typename T>
	struct TupleElement;

	template<uint64 N, typename T, typename... TYPES>
	struct TupleElement<N, Tuple<T, TYPES...>> : TupleElement<N - 1, Tuple<TYPES...>> {};

	template<typename T, typename... TYPES>
	struct TupleElement<0, Tuple<T, TYPES...>> { using type = T; };

	template <uint64 N>
	struct TupleAccessor
	{
		template <class... TYPES>
		static typename TupleElement<N, Tuple<TYPES...> >::type& Get(Tuple<TYPES...>& t)
		{
			return TupleAccessor<N - 1>::Get(t.rest);
		}

		template <class... TYPES>
		static const typename TupleElement<N, Tuple<TYPES...> >::type& Get(const Tuple<TYPES...>& t)
		{
			return TupleAccessor<N - 1>::Get(t.rest);
		}

		template <class... TYPES>
		static typename TupleElement<N, Tuple<TYPES...> >::type&& Get(Tuple<TYPES...>&& t)
		{
			return TupleAccessor<N - 1>::Get(t.rest);
		}

		template <class... TYPES>
		static const typename TupleElement<N, Tuple<TYPES...> >::type&& Get(const Tuple<TYPES...>&& t)
		{
			return TupleAccessor<N - 1>::Get(t.rest);
		}
	};

	template <>
	struct TupleAccessor<0>
	{
		template <class... Ts>
		static typename TupleElement<0, Tuple<Ts...> >::type& Get(Tuple<Ts...>& t)
		{
			return t.element;
		}

		template <class... Ts>
		static const typename TupleElement<0, Tuple<Ts...> >::type& Get(const Tuple<Ts...>& t)
		{
			return t.element;
		}

		template <class... Ts>
		static typename TupleElement<0, Tuple<Ts...> >::type&& Get(Tuple<Ts...>&& t)
		{
			return t.element;
		}

		template <class... Ts>
		static const typename TupleElement<0, Tuple<Ts...> >::type&& Get(const Tuple<Ts...>&& t)
		{
			return t.element;
		}
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

	template<uint64...>
	struct Indices {};

	template<uint64 N, uint64... S>
	struct BuildIndices : BuildIndices<N - 1, N - 1, S...> {};

	template<uint64... S>
	struct BuildIndices<0, S...> : Indices<S...> {};
}