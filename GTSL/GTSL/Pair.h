#pragma once

template <typename A, typename B>
struct Pair
{
	constexpr A First;
	constexpr B Second;

	Pair() = default;

	constexpr Pair(const A& first, const B& second) noexcept : First(first), Second(second)
	{
	}
};
