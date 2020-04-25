#pragma once

namespace GTSL
{
	template<typename T>
	class Ranger
	{
		T* from = 0, * to = 0;

	public:
		using type = T;
		using type_pointer = T*;

		constexpr Ranger(T* start, T* end) noexcept : from(start), to(end)
		{
		}

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

		[[nodiscard]] constexpr uint64 Bytes() const noexcept { return (to - from) / sizeof(type); }
		
		[[nodiscard]] constexpr void* Data() noexcept { return from; }
		[[nodiscard]] constexpr const void* Data() const noexcept { return from; }
	};
}