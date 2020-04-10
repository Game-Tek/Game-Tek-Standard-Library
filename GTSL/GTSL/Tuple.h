#pragma once
#include "Core.h"

namespace GTSL
{
	template<size_t I, typename T>
	class TupleElement
	{
		T element;
	public:
		TupleElement(const T& e) : element(e) {}
		TupleElement(T&& e) : element(GTSL::MakeTransferReference(e)) {}

		T& Get() { return element; }
	};

	template<size_t I, typename T, typename ...TYPES>
	class TupleRecursive : public TupleElement<I, T>, TupleRecursive<I + 1, TYPES...>
	{
	public:
		template<typename CL, typename ...ARGS>
		TupleRecursive(CL &&arg, ARGS&& ...args) : TupleElement<I, CL>(GTSL::MakeForwardReference<CL>(arg)), TupleRecursive<I + 1, TYPES...>(GTSL::MakeForwardReference<ARGS>(args) ...)
		{}
	};

	template <size_t I, typename T, typename ...ARGS>
	struct ExtractTypeAt
	{
		using type = typename ExtractTypeAt<I - 1, ARGS...>::type;
	};

	template<typename T, typename ...ARGS>
	struct ExtractTypeAt<0, T, ARGS...>
	{
		using type = T;
	};
	
	template<typename T, typename ...TYPES>
	class Tuple : public TupleRecursive<0, T, TYPES...>
	{
		template <size_t I, typename ...ARGS>
		bool compareTuple(Tuple<ARGS...>& t1, Tuple<ARGS...>& t2)
		{
			if constexpr (I == 0)
			{
				return Get<0>(t1) == Get<0>(t2);
			}
			else
			{
				return Get<I>(t1) == Get<I>(t2) && compareTuple<I - 1>(t1, t2);
			}
		}
	public:
		template<typename ...ARGS>
		Tuple(ARGS&& ...args) : TupleRecursive<0, T, TYPES...>(GTSL::MakeForwardReference<ARGS>(args) ...)
		{}
		
		template<size_t I, typename ...ARGS>
		auto& Get(Tuple<ARGS...>& t)
		{
			return (static_cast<TupleElement<I, typename ExtractTypeAt<I, ARGS...>::type> &>(t)).Get();
		}

		template <typename... ARGS>
		bool operator==(Tuple<ARGS...>& t1, Tuple<ARGS...>& t2)
		{
			return compareTuple<sizeof...(ARGS) - 1>(t1, t2);
		}
	};
}
