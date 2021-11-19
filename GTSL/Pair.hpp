#pragma once

namespace GTSL {
	template <typename A, typename B>
	struct Pair {
		A First; B Second;

		Pair() = default;
		constexpr Pair(const A& first, const B& second) noexcept : First(first), Second(second) {}
		constexpr Pair(A&& first, B&& second) noexcept : First(MoveRef(first)), Second(MoveRef(second)) {}
	};

	template<typename T>
	Pair(const T& a)->Pair<T, T>;

	template<typename T>
	Pair(T&& a)->Pair<T, T>;
}
