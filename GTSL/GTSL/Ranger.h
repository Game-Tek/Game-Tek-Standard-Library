#pragma once

template<typename T>
class Ranger
{
	T* from = 0,* to = 0;
	
public:
	constexpr Ranger(const T* start, const T* end) noexcept : from(start), to(end)
	{
	}

	constexpr Ranger(const size_t length, T* start) noexcept : from(start), to(start + length)
	{
	}
	
	constexpr T* begin() noexcept { return from; }
	[[nodiscard]] constexpr const T* begin() const noexcept { return from; }
	constexpr T* end() noexcept { return to; }
	[[nodiscard]] constexpr const T* end() const noexcept { return to; }
};