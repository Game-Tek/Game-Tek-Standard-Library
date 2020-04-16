#pragma once

#include "Core.h"

#include <immintrin.h>

namespace GTSL
{

	class int32_4
	{
		__m128i Data;

	public:
		int32_4 operator+(const int32_4& other) const
		{
			return _mm_add_ps(Data, other.Data);
		}

		int32_4& operator+=(const int32_4& other)
		{
			Data = _mm_add_ps(Data, other.Data);
			return *this;
		}

		int32_4 operator-(const int32_4& other) const
		{
			return _mm_sub_ps(Data, other.Data);
		}

		int32_4& operator-=(const int32_4& other)
		{
			Data = _mm_sub_ps(Data, other.Data);
			return *this;
		}

		int32_4 operator*(const int32_4& other) const
		{
			return _mm_mul_ps(Data, other.Data);
		}

		int32_4& operator*=(const int32_4& other)
		{
			Data = _mm_mul_ps(Data, other.Data);
			return *this;
		}

		int32_4 operator/(const int32_4& other) const
		{
			return _mm_div_ps(Data, other.Data);
		}

		int32_4& operator/=(const int32_4& other)
		{
			Data = _mm_div_ps(Data, other.Data);
			return *this;
		}
	};
}