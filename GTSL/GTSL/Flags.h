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
		
		constexpr Flags operator| (const Flags flag) const { return Flags(value | flag.value); }
		constexpr Flags& operator|= (const Flags flag) { value |= flag.value; return *this; }
		
		constexpr Flags operator&(const Flags flag) const { return Flags(value & flag.value); }
		constexpr operator bool() const { return value; }
		constexpr bool operator==(const Flags other) const { return value == other.value; }
	private:
		friend Flags;
		T value = 0;
	};
}