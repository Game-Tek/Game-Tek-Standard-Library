#pragma once

#include "Core.hpp"

namespace GTSL
{
	template<typename ST, std::uint64_t NUM, std::uint64_t DEN>
	class RatioUnit
	{
	public:
		using storing_type = ST;

		static constexpr ST NUMERATOR = NUM;
		static constexpr ST DENOMINATOR = DEN;

		RatioUnit() = default;

		explicit constexpr RatioUnit(const storing_type time) : m_count(time) {}

		template<typename RET, class R>
		constexpr RET As() const
		{
			if constexpr (R::DENOMINATOR > DENOMINATOR)
			{
				if constexpr (NUMERATOR == R::NUMERATOR)
				{
					return m_count * (static_cast<RET>(R::DENOMINATOR) / static_cast<RET>(DENOMINATOR));
				}
				else 
				{
					return m_count * static_cast<RET>(R::NUMERATOR / R::DENOMINATOR);
				}
			}
			else if constexpr (R::DENOMINATOR < DENOMINATOR)
			{
				if constexpr (NUMERATOR == R::NUMERATOR)
				{
					return m_count / static_cast<RET>(DENOMINATOR);
				}
				else {
					return m_count / static_cast<RET>(NUMERATOR / DENOMINATOR);
				}
			}
			else 
			{
				if constexpr (R::NUMERATOR < NUMERATOR)
				{
					return m_count * static_cast<RET>(NUMERATOR);
				}
				else 
				{
					return m_count;
				}

				return static_cast<RET>(m_count);
			}

			return 0;
		}

		template<uint64 N, uint64 D>
		constexpr RatioUnit(RatioUnit<ST, N, D> other) : m_count(other.template As<ST, RatioUnit<ST, NUM, DEN>>()) {}

		constexpr explicit operator storing_type() const { return m_count; }

		constexpr RatioUnit operator+(const RatioUnit& other) const { return RatioUnit(this->m_count + other.m_count); }
		constexpr RatioUnit operator-(const RatioUnit& other) const { return RatioUnit(this->m_count - other.m_count); }
		constexpr RatioUnit operator*(const RatioUnit& other) const { return RatioUnit(this->m_count * other.m_count); }
		constexpr RatioUnit operator/(const RatioUnit& other) const { return RatioUnit(this->m_count / other.m_count); }

		constexpr RatioUnit& operator+=(const RatioUnit& other) { this->m_count += other.m_count; return *this; }
		constexpr RatioUnit& operator-=(const RatioUnit& other) { this->m_count -= other.m_count; return *this; }
		constexpr RatioUnit& operator*=(const RatioUnit& other) { this->m_count *= other.m_count; return *this; }
		constexpr RatioUnit& operator/=(const RatioUnit& other) { this->m_count /= other.m_count; return *this; }

		constexpr bool operator>(const RatioUnit& other) const { return this->m_count > other.m_count; }
		constexpr bool operator<(const RatioUnit& other) const { return this->m_count < other.m_count; }

		constexpr bool operator==(const RatioUnit& other) const { return this->m_count == other.m_count; }
	protected:
		ST m_count{ 0 };
	};
}