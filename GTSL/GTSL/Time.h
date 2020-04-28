#pragma once

#include "Core.h"

namespace GTSL
{
	template<typename ST, uint64 NUM, uint64 RATIO>
	class TimeUnit
	{
	protected:
		ST count{ 0 };
		
	public:
		using storing_type = ST;

		TimeUnit() = default;
		
		TimeUnit(const storing_type time) : count(time * NUM) {}
		
		template<typename T>
		operator T() const { return count * static_cast<T>((NUM / RATIO)); }

		template<typename S, storing_type N, storing_type R>
		operator TimeUnit<S, N, R>() const { return TimeUnit<S, N, R>(count * (N / R)); }

		template<typename RET, typename S, storing_type N, storing_type R>
		RET As(const TimeUnit<S, N, R>& other) const { return count * static_cast<RET>((N / R)); }

		storing_type Count() const { return count; }
		
		template<typename S, storing_type N, storing_type R>
		TimeUnit operator+(const TimeUnit<S, N, R>& other) const { return this->count + (other.count * (N / R)); }
		template<typename S, storing_type N, storing_type R>
		TimeUnit operator-(const TimeUnit<S, N, R>& other) const { return this->count - (other.count * (N / R)); }
		template<typename S, storing_type N, storing_type R>
		TimeUnit operator*(const TimeUnit<S, N, R>& other) const { return this->count * (other.count * (N / R)); }
		template<typename S, storing_type N, storing_type R>
		TimeUnit operator/(const TimeUnit<S, N, R>& other) const { return this->count / (other.count * (N / R)); }

		template<typename S, storing_type N, storing_type R>
		TimeUnit& operator+=(const TimeUnit<S, N, R>& other) { this->count += (other.count * (N / R)); return *this; }
		template<typename S, storing_type N, storing_type R>
		TimeUnit& operator-=(const TimeUnit<S, N, R>& other) { this->count -= (other.count * (N / R)); return *this; }
		template<typename S, storing_type N, storing_type R>
		TimeUnit& operator*=(const TimeUnit<S, N, R>& other) { this->count *= (other.count * (N / R)); return *this; }
		template<typename S, storing_type N, storing_type R>
		TimeUnit& operator/=(const TimeUnit<S, N, R>& other) { this->count /= (other.count * (N / R)); return *this; }

		template<typename S, storing_type N, storing_type R>
		bool operator>(const TimeUnit<S, N, R>& other) const { return this->count > (other.count * (N / R)); }

		template<typename S, storing_type N, storing_type R>
		bool operator<(const TimeUnit<S, N, R>& other) const { return this->count < (other.count * (N / R)); }
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