#pragma once

#include "Core.h"

namespace GTSL
{
	template<typename ST, uint64 NUM, uint64 DEN>
	class RatioUnit
	{
	public:
		using storing_type = ST;

		static constexpr uint64 NUMERATOR = NUM;
		static constexpr uint64 DENOMINATOR = DEN;
		
		RatioUnit() = default;

		explicit constexpr RatioUnit(const storing_type time) : count(time) {}

		template<storing_type N, storing_type R>
		constexpr RatioUnit(const RatioUnit<ST, N, R>& other) : count(other.count * (N / R))
		{
			static_assert(N >= NUM && R <= DENOMINATOR, "Converting to a larger unit will yield a lossy transformation.");
		}

		constexpr RatioUnit(const RatioUnit& other) : count(other.count) {}

		template<typename S, storing_type N, storing_type R>
		constexpr operator RatioUnit<S, N, R>() const
		{
			static_assert(N >= NUM, "Converting to a larger unit will yield a lossy transformation.");
			return RatioUnit<S, N, R>(count * (N / R));
		}

		template<typename RET, class R>
		RET As()
		{
			if constexpr ((NUM % DEN) == 0)
			{
				auto value = count;
				value *= NUM / DEN;

				if constexpr ((R::NUMERATOR % R::DENOMINATOR) == 0)
				{
					return RET(value * (R::NUMERATOR / R::DENOMINATOR));
				}
			}
			
			auto value = static_cast<RET>(count);
			value *= (static_cast<RET>(NUM) / static_cast<RET>(DEN));
			return value * (static_cast<RET>(R::NUMERATOR) / static_cast<RET>(R::DENOMINATOR));
		}

		operator storing_type() const { return count; }
		
		constexpr storing_type GetCount() const { return count; }

		constexpr RatioUnit operator+(const RatioUnit& other) const { return RatioUnit(this->count + other.count); }
		constexpr RatioUnit operator-(const RatioUnit& other) const { return RatioUnit(this->count - other.count); }
		constexpr RatioUnit operator*(const RatioUnit& other) const { return RatioUnit(this->count * other.count); }
		constexpr RatioUnit operator/(const RatioUnit& other) const { return RatioUnit(this->count / other.count); }

		constexpr RatioUnit& operator+=(const RatioUnit& other) { this->count += other.count; return *this; }
		constexpr RatioUnit& operator-=(const RatioUnit& other) { this->count -= other.count; return *this; }
		constexpr RatioUnit& operator*=(const RatioUnit& other) { this->count *= other.count; return *this; }
		constexpr RatioUnit& operator/=(const RatioUnit& other) { this->count /= other.count; return *this; }

		constexpr bool operator>(const RatioUnit& other) const { return this->count > other.count; }
		constexpr bool operator<(const RatioUnit& other) const { return this->count < other.count; }

	protected:
		ST count{ 0 };

		friend class RatioUnit;
	};
}
