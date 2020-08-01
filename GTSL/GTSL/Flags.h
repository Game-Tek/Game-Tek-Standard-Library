#pragma once

namespace GTSL
{
	template<typename T>
	struct Flags
	{
		constexpr Flags() = default;
		constexpr Flags(const T val) noexcept : value(val) {}
		constexpr Flags(const Flags& other) noexcept : value(other.value) {}
		~Flags() = default;

		constexpr Flags& operator=(const T val) { value = val; return *this; }
		constexpr Flags& operator=(const Flags& val) { value = val.value; return *this; }
		
		constexpr operator T() const { return value; }
		constexpr operator T& () { return value; }

		using value_type = T;
		using type = Flags;
	private:
		friend Flags;
		T value = 0;
	};
}