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
		
		constexpr explicit operator T() const { return value; }
		constexpr explicit operator T& () { return value; }

		using value_type = T;
		using type = Flags;
		
		Flags operator| (const Flags flag) { return value | flag; }
		Flags& operator|= (const Flags flag) { return value |= flag; }
	private:
		friend Flags;
		T value = 0;
	};
}