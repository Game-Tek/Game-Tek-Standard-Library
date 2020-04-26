#pragma once

#include "Core.h"

namespace GTSL
{
	template<uint64 NUM, uint64 RATIO>
	class UnsignedTimeUnit
	{
	protected:
		uint64 count{ 0 };

	public:
		using storing_type = uint64;

		UnsignedTimeUnit(const storing_type time) : count(time) {}
		
		template<typename T>
		operator T() const { return static_cast<T>(count) / static_cast<T>(RATIO); }

		template<uint64 N, uint64 R>
		operator UnsignedTimeUnit<N, R>() const { return UnsignedTimeUnit<N, R>(count * (N / R)); }

		template<uint64 N, uint64 R>
		UnsignedTimeUnit operator+(const UnsignedTimeUnit<N, R>& other) { return this->count + (other.count * (N / R)); }
		template<uint64 N, uint64 R>
		UnsignedTimeUnit operator-(const UnsignedTimeUnit<N, R>& other) { return this->count - (other.count * (N / R)); }
		template<uint64 N, uint64 R>
		UnsignedTimeUnit operator*(const UnsignedTimeUnit<N, R>& other) { return this->count * (other.count * (N / R)); }
		template<uint64 N, uint64 R>
		UnsignedTimeUnit operator/(const UnsignedTimeUnit<N, R>& other) { return this->count / (other.count * (N / R)); }

		template<uint64 N, uint64 R>
		UnsignedTimeUnit& operator+=(const UnsignedTimeUnit<N, R>& other) { this->count += (other.count * (N / R)); return *this; }
		template<uint64 N, uint64 R>
		UnsignedTimeUnit& operator-=(const UnsignedTimeUnit<N, R>& other) { this->count -= (other.count * (N / R)); return *this; }
		template<uint64 N, uint64 R>
		UnsignedTimeUnit& operator*=(const UnsignedTimeUnit<N, R>& other) { this->count *= (other.count * (N / R)); return *this; }
		template<uint64 N, uint64 R>
		UnsignedTimeUnit& operator/=(const UnsignedTimeUnit<N, R>& other) { this->count /= (other.count * (N / R)); return *this; }
	};

	template<uint64 RATIO>
	class SignedTimeUnit
	{
	protected:
		int64 count{ 0 };

	public:
		using storing_type = int64;

		SignedTimeUnit(const storing_type time) : count(time) {}
		
		template<uint64 R>
		operator SignedTimeUnit<R>() const { return SignedTimeUnit<R>(count / R); }

		SignedTimeUnit operator+(const SignedTimeUnit& other) { return this->count + other.count; }
		SignedTimeUnit operator-(const SignedTimeUnit& other) { return this->count - other.count; }
		SignedTimeUnit operator*(const SignedTimeUnit& other) { return this->count * other.count; }
		SignedTimeUnit operator/(const SignedTimeUnit& other) { return this->count / other.count; }

		SignedTimeUnit& operator+=(const SignedTimeUnit& other) { this->count += other.count; return *this; }
		SignedTimeUnit& operator-=(const SignedTimeUnit& other) { this->count -= other.count; return *this; }
		SignedTimeUnit& operator*=(const SignedTimeUnit& other) { this->count *= other.count; return *this; }
		SignedTimeUnit& operator/=(const SignedTimeUnit& other) { this->count /= other.count; return *this; }
	};
	
	using SignedNanoseconds  = SignedTimeUnit<1000000000>;
	using SignedMicroseconds = SignedTimeUnit<1000000>;
	using SignedMilliseconds = SignedTimeUnit<1000>;
	using SignedSeconds      = SignedTimeUnit<1>;

	using Nanoseconds  = UnsignedTimeUnit<1, 1000000000>;
	using Microseconds = UnsignedTimeUnit<1, 1000000>;
	using Milliseconds = UnsignedTimeUnit<1, 1000>;
	using Seconds      = UnsignedTimeUnit<1, 1>;
	
	//template<typename T, uint64 ratio>
	//T TimeUnitCast(const UnsignedTimeUnit<ratio>& timeUnit) { return T(timeUnit.count / ratio); }
	//
	//template<typename T, uint64 ratio>
	//T TimeUnitCast(const SignedTimeUnit<ratio>& timeUnit) { return T(timeUnit.count / ratio); }
}