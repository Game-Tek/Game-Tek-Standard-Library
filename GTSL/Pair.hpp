#pragma once

#include "Core.h"

namespace GTSL {
	template <typename A, typename B>
	struct Pair {
		A First; B Second;

		Pair() = default;
		constexpr Pair(const A& first, const B& second) noexcept : First(first), Second(second) {}
		constexpr Pair(A&& first, B&& second) noexcept : First(GTSL::MoveRef(first)), Second(GTSL::MoveRef(second)) {}
	};

	template<typename T>
	Pair(const T& a)->Pair<T, T>;

	template<typename T>
	Pair(T&& a)->Pair<T, T>;
}
