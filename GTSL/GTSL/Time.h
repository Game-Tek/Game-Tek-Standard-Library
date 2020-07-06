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

		constexpr RatioUnit(const RatioUnit& other) : count(other.count) {}

		template<typename S, storing_type N, storing_type R>
		constexpr operator RatioUnit<S, N, R>() const
		{
			static_assert(R > RATIO, "Converting to a larger unit will yield a lossy transformation.");
			return RatioUnit<S, N, R>(count * (N / R));
		}

		template<typename RET, typename S1, storing_type N1, storing_type R1, typename S2, storing_type N2, storing_type R2>
		static constexpr RET As(const RatioUnit<S1, N1, R1>& a, const RatioUnit<S2, N2, R2>& b) { return static_cast<RET>(a.count) * static_cast<RET>((N2 / R2)); }

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
	};
	
	using SignedNanoseconds  = RatioUnit<int64, 1, 1000000000>;
	using SignedMicroseconds = RatioUnit<int64, 1, 1000000>;
	using SignedMilliseconds = RatioUnit<int64, 1, 1000>;
	using SignedSeconds      = RatioUnit<int64, 1, 1>;

	using Nanoseconds  = RatioUnit<uint64, 1, 1000000000>;
	using Microseconds = RatioUnit<uint64, 1, 1000000>;
	using Milliseconds = RatioUnit<uint64, 1, 1000>;
	using Seconds      = RatioUnit<uint64, 1, 1>;
	using Minutes      = RatioUnit<uint64, 60, 1>;
	using Hour		   = RatioUnit<uint64, 3600, 1>;
	using Day		   = RatioUnit<uint64, 86400, 1>;
	
	//template<typename T, uint64 ratio>
	//T TimeUnitCast(const UnsignedTimeUnit<ratio>& timeUnit) { return T(timeUnit.count / ratio); }
	//
	//template<typename T, uint64 ratio>
	//T TimeUnitCast(const SignedTimeUnit<ratio>& timeUnit) { return T(timeUnit.count / ratio); }
}