#pragma once

#include <concepts>

#include "Core.h"

namespace GTSL {
	template <typename A, typename B>
	struct Pair {
		A First; B Second;

		Pair() = default;
		constexpr Pair(const A& first, const B& second) noexcept requires(std::copy_constructible<A>&& std::copy_constructible<B>) : First(first), Second(second) {}
		constexpr Pair(A&& first, B&& second) noexcept : First(GTSL::MoveRef(first)), Second(GTSL::MoveRef(second)) {}

		Pair(const Pair& other) noexcept : First(other.First), Second(other.Second) {}
		Pair(Pair&& other) noexcept : First(GTSL::MoveRef(other.First)), Second(GTSL::MoveRef(other.Second)) {}

		Pair& operator=(const Pair& other) noexcept {
			First = other.First; Second = other.Second;
			return *this;
		}

		Pair& operator=(Pair&& other) noexcept {
			First = GTSL::MoveRef(other.First);
			Second = GTSL::MoveRef(other.Second);
			return *this;
		}

		bool operator==(const Pair&) const = default;
	};

	template<typename T>
	Pair(const T& a)->Pair<T, T>;

	template<typename T>
	Pair(T&& a)->Pair<T, T>;
}