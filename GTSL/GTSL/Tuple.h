#pragma once

#include "Core.h"

namespace GTSL
{
	template <uint64 I, typename T, typename... ARGS>
	struct ExtractTypeAt
	{
		using type = typename ExtractTypeAt<I - 1, ARGS...>::type;
	};

	template<typename T, typename ...ARGS>
	struct ExtractTypeAt<0, T, ARGS...>
	{
		using type = T;
	};

	template<typename... ARGS> class Tuple;

	template<>
	class Tuple<> {};

	template<typename T, typename... TYPES>
	class Tuple<T, TYPES...> : public Tuple<TYPES...>
	{
	public:
		template<typename... ARGS>
		Tuple(ARGS&&... args) : Tuple<T, TYPES...>(GTSL::MakeForwardReference<ARGS>(args)...)
		{}

		Tuple(const Tuple& other) = default;
		Tuple(Tuple&& other) = default;
		Tuple& operator=(const Tuple& other) = default;
		Tuple& operator=(Tuple&& other) = default;

		template <typename... ARGS>
		friend bool operator==(Tuple<ARGS...>& t1, Tuple<ARGS...>& t2)
		{
			return compareTuple<sizeof...(ARGS) - 1>(t1, t2);
		}

	private:
		T element;
		
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
	};

	template <class... ARGS>
	Tuple(ARGS...)->Tuple<ARGS...>;

	template<uint64 ...>
	struct Indices {};

	template<uint64 N, uint64... S>
	struct BuildIndices : BuildIndices<N - 1, N - 1, S...> { };

	template<uint64... S>
	struct BuildIndices<0, S...> : Indices<S...> {};
}