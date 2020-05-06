#pragma once

#include "Core.h"

namespace GTSL
{
	template<typename ST, uint64 NUM, uint64 RATIO>
	class TimeUnit
	{
	public:
		using storing_type = ST;
	protected:
		ST count{ 0 };
		
	public:
		TimeUnit() = default;

		explicit constexpr TimeUnit(const storing_type time) : count(time) {}

		constexpr TimeUnit(const TimeUnit& other) : count(other.count) {}

		template<typename S, storing_type N, storing_type R>
		constexpr operator TimeUnit<S, N, R>() const
		{
			static_assert(R > RATIO, "Converting to a larger unit will yield a lossy transformation.");
			return TimeUnit<S, N, R>(count * (N / R));
		}

		template<typename RET, typename S1, storing_type N1, storing_type R1, typename S2, storing_type N2, storing_type R2>
		static constexpr RET As(const TimeUnit<S1, N1, R1>& a, const TimeUnit<S2, N2, R2>& b) { return static_cast<RET>(a.count) * static_cast<RET>((N2 / R2)); }

		constexpr storing_type GetCount() const { return count; }
		
		constexpr TimeUnit operator+(const TimeUnit& other) const { return TimeUnit(this->count + other.count); }
		constexpr TimeUnit operator-(const TimeUnit& other) const { return TimeUnit(this->count - other.count); }
		constexpr TimeUnit operator*(const TimeUnit& other) const { return TimeUnit(this->count * other.count); }
		constexpr TimeUnit operator/(const TimeUnit& other) const { return TimeUnit(this->count / other.count); }
		
		constexpr TimeUnit& operator+=(const TimeUnit& other) { this->count += other.count; return *this; }
		constexpr TimeUnit& operator-=(const TimeUnit& other) { this->count -= other.count; return *this; }
		constexpr TimeUnit& operator*=(const TimeUnit& other) { this->count *= other.count; return *this; }
		constexpr TimeUnit& operator/=(const TimeUnit& other) { this->count /= other.count; return *this; }
		
		constexpr bool operator>(const TimeUnit& other) const { return this->count > other.count; }
		constexpr bool operator<(const TimeUnit& other) const { return this->count < other.count; }
	};
	
	using SignedNanoseconds  = TimeUnit<int64, 1, 1000000000>;
	using SignedMicroseconds = TimeUnit<int64, 1, 1000000>;
	using SignedMilliseconds = TimeUnit<int64, 1, 1000>;
	using SignedSeconds      = TimeUnit<int64, 1, 1>;

	using Nanoseconds  = TimeUnit<uint64, 1, 1000000000>;
	using Microseconds = TimeUnit<uint64, 1, 1000000>;
	using Milliseconds = TimeUnit<uint64, 1, 1000>;
	using Seconds      = TimeUnit<uint64, 1, 1>;
	using Minutes      = TimeUnit<uint64, 60, 1>;
	using Hour		   = TimeUnit<uint64, 3600, 1>;
	using Day		   = TimeUnit<uint64, 86400, 1>;
	
	//template<typename T, uint64 ratio>
	//T TimeUnitCast(const UnsignedTimeUnit<ratio>& timeUnit) { return T(timeUnit.count / ratio); }
	//
	//template<typename T, uint64 ratio>
	//T TimeUnitCast(const SignedTimeUnit<ratio>& timeUnit) { return T(timeUnit.count / ratio); }
}