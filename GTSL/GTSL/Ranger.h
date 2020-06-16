#pragma once

#include "Core.h"

namespace GTSL
{
	template<typename T>
	class Ranger
	{
	public:
		using type = T;
		using type_pointer = T*;

		Ranger() = default;

		constexpr Ranger(T* start, T* end) noexcept : from(start), to(end)
		{
		}

		constexpr Ranger(size_t length, T* start) noexcept : from(start), to(start + length)
		{
		}

		constexpr T* begin() noexcept { return from; }
		[[nodiscard]] constexpr T* begin() const noexcept { return from; }
		constexpr T* end() noexcept { return to; }
		[[nodiscard]] constexpr T* end() const noexcept { return to; }

		[[nodiscard]] constexpr uint64 Bytes() const noexcept { return ((to - from) * sizeof(type)); }

		[[nodiscard]] uint64 ElementCount() const { return to - from; }

		T* operator+(const uint64 i) const { return this->from + i; }
		T* operator*(const uint64 i) const { return this->from * i; }
		T* operator/(const uint64 i) const { return this->from / i; }

		operator T*() { return this->from; }
		operator T*() const { return this->from; }
		
		T& operator[](const uint64 i) { return this->from[i]; }
		//T& operator[](const uint64 i) const { return this->from[i]; }

	protected:
		T* from = 0, * to = 0;
	};
}
