#pragma once

#include "Core.h"

namespace GTSL
{
	template<typename T>
	class Ranger
	{
	protected:
		T* from = 0, * to = 0;

	public:
		using type = T;
		using type_pointer = T*;

		Ranger() = default;

		constexpr Ranger(T* start, T* end) noexcept : from(start), to(end)
		{
		}

		constexpr Ranger(size_t length, const T* start) noexcept : from(const_cast<T*>(start)), to(const_cast<T*>(start) + length)
		{
		}

		constexpr T* begin() noexcept { return from; }
		[[nodiscard]] constexpr const T* begin() const noexcept { return from; }
		constexpr T* end() noexcept { return to; }
		[[nodiscard]] constexpr const T* end() const noexcept { return to; }

		[[nodiscard]] constexpr uint64 Bytes() const noexcept { return ((to - from) * sizeof(type)); }

		[[nodiscard]] uint64 ElementCount() const { return to - from; }
		[[nodiscard]] constexpr void* Data() noexcept { return from; }
		[[nodiscard]] constexpr const void* Data() const noexcept { return from; }

		T* operator+(const uint64 i) const { return this->from + i; }

		T& operator[](const uint64 i) const { return this->from[i]; }
	};
}
