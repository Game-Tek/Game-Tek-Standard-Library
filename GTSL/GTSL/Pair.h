#pragma once

template <typename A, typename B>
struct Pair
{
	A First;
	B Second;

	Pair() = default;

	Pair(const A& first, const B& second) : First(first), Second(second)
	{
	}
};
