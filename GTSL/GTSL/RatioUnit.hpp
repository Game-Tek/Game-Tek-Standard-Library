#pragma once

#include "Core.h"

namespace GTSL
{
	template<typename ST, uint64 NUM, uint64 RATIO>
	class RatioUnit
	{
	public:
		using storing_type = ST;

		RatioUnit() = default;

		explicit constexpr RatioUnit(const storing_type time) : count(time) {}

		template<storing_type N, storing_type R>
		constexpr RatioUnit(const RatioUnit<ST, N, R>& other) : count(other.count * (N / R))
		{
			static_assert(N >= NUM && R <= RATIO, "Converting to a larger unit will yield a lossy transformation.");
		}

		constexpr RatioUnit(const RatioUnit& other) : count(other.count) {}

		template<typename S, storing_type N, storing_type R>
		constexpr operator RatioUnit<S, N, R>() const
		{
			static_assert(N >= NUM, "Converting to a larger unit will yield a lossy transformation.");
			return RatioUnit<S, N, R>(count * (N / R));
		}

		template<typename RET, typename S1, storing_type N1, storing_type R1, typename S2, storing_type N2, storing_type R2>
		static constexpr RET As(const RatioUnit<S1, N1, R1>& a, const RatioUnit<S2, N2, R2>& b) { return static_cast<RET>(a.count) * static_cast<RET>((N2 / R2)); }

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
