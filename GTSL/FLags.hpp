#pragma once

namespace GTSL
{
	template<typename T, typename TAG>
	struct Flags
	{
		constexpr Flags() = default;
		constexpr explicit Flags(const T val) noexcept : m_value(val) {}
		constexpr Flags(const Flags& other) noexcept : m_value(other.m_value) {}

		~Flags() = default;

		constexpr Flags& operator=(const Flags& other) { m_value = other.m_value; return *this; }

		constexpr operator T() const { return m_value; }
		constexpr operator T& () { return m_value; }

		using value_type = T;
		using type = Flags;

		constexpr Flags operator| (const Flags flag) const { return Flags(m_value | flag.m_value); }
		constexpr Flags& operator|= (const Flags flag) { m_value |= flag.m_value; return *this; }

		constexpr Flags operator~() const { return Flags(~m_value); }

		constexpr explicit operator bool() const { return m_value; }
		constexpr Flags operator&(const Flags flag) const { return Flags(m_value & flag.m_value); }
		constexpr bool operator==(const Flags other) const { return m_value == other.m_value; }
	private:
		T m_value = 0;
	};
}