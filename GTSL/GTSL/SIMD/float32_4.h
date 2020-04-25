#pragma once

#include <immintrin.h>

namespace GTSL
{
	class float32_4
	{
		__m128 vector;

		float32_4(const __m128& _m128) : vector(_m128)
		{
		}

	public:
		float32_4() : vector(_mm_setzero_ps())
		{
		}

		explicit float32_4(const float a) : vector(_mm_set_ps1(a))
		{
		}

		float32_4(const float x, const float y, const float z, const float w) : vector(_mm_set_ps(w, z, y, x))
		{
		}

		~float32_4() = default;

		static float32_4 MakeFromAligned(const float* data) { return _mm_load_ps(data); }
		static float32_4 MakeFromUnaligned(const float* data) { return _mm_loadu_ps(data); }

		operator __m128() const { return vector; }

		void SetAligned(const float* data) { vector = _mm_load_ps(data); }
		void SetUnaligned(const float* data) { vector = _mm_loadu_ps(data); }

		/**
		 * \brief Sets all of this vector's components as a.
		 * \param a float to set all of this vector's components as.
		 * \return Returns a reference to itself.
		 */
		float32_4& operator=(const float a)
		{
			vector = _mm_set_ps1(a);
			return *this;
		}

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into unaligned memory.
		void CopyToUnalignedData(float* data) const
		{
			_mm_storeu_ps(data, vector);
		}

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into aligned memory.
		void CopyToAlignedData(float* data) const
		{
			_mm_store_ps(data, vector);
		}

		//Shuffle single-precision (32-bit) floating-point elements in a using the control in imm8, and store the results in dst.
		template<const unsigned A, const unsigned B, const unsigned C, const unsigned D>
		[[nodiscard]] static float32_4 Shuffle(const float32_4& a, const float32_4& b)
		{
			return _mm_shuffle_ps(a.vector, b.vector, _MM_SHUFFLE(A, B, C, D));
		}

		static float32_4 Abs(const float32_4& a)
		{
			return _mm_andnot_ps(a.vector, float32_4(1.0f));
		}

		static float32_4 HorizontalAdd(const float32_4& a, const float32_4& b)
		{
			return _mm_hadd_ps(a.vector, b.vector);
		}

		//Horizontally add adjacent pairs of single - precision(32 - bit) floating - point elements in a and B, and pack the results in dst.
		[[nodiscard]] static float32_4 HorizontalSub(const float32_4& a, const float32_4& b)
		{
			return _mm_hsub_ps(a.vector, b.vector);
		}

		//Alternatively add and subtract packed single-precision (32-bit) floating-point elements in a to/from packed elements in B, and store the results in dst
		[[nodiscard]] static float32_4 Add13Sub02(const float32_4& a, const float32_4& b)
		{
			return _mm_addsub_ps(a.vector, b.vector);
		}

		//Conditionally multiply the packed single-precision (32-bit) floating-point elements in a and B using the high 4 bits in imm8, sum the four products, and conditionally store the sum in dst using the low 4 bits of imm8.
		[[nodiscard]] static float32_4 DotProduct(const float32_4& a, const float32_4& b)
		{
			return _mm_dp_ps(a.vector, b.vector, 0xff);
		}

		static void Transpose(float32_4& a, float32_4& b, float32_4& c, float32_4& d)
		{
			_MM_TRANSPOSE4_PS(a, b, c, d);
		}

		[[nodiscard]] float32_4 SquareRoot() const
		{
			return _mm_sqrt_ps(vector);
		}

		//Compute the square root of the lower single-precision (32-bit) floating-point element in a, store the result in the lower element of dst, and copy the upper 3 packed elements from a to the upper elements of dst.
		[[nodiscard]] float32_4 SquareRootToLower(const float32_4& a) const
		{
			return _mm_sqrt_ss(vector);
		}

		//Returns the first element of the vector.
		[[nodiscard]] float GetX() const
		{
			return _mm_cvtss_f32(vector);
		}

		[[nodiscard]] float GetY() const
		{
			alignas(16) float v[4];
			_mm_store_ps(v, vector);
			return v[1];
		}

		[[nodiscard]] float GetZ() const
		{
			alignas(16) float v[4];
			_mm_store_ps(v, vector);
			return v[2];
		}

		[[nodiscard]] float GetW() const
		{
			alignas(16) float v[4];
			_mm_store_ps(v, vector);
			return v[3];
		}

		[[nodiscard]] __m128i Toint4() const
		{
			return _mm_cvtps_epi32(vector);
		}

		float32_4 operator+(const float32_4& other) const
		{
			return _mm_add_ps(vector, other.vector);
		}

		float32_4& operator+=(const float32_4& other)
		{
			vector = _mm_add_ps(vector, other.vector);
			return *this;
		}

		float32_4 operator-(const float32_4& other) const
		{
			return _mm_sub_ps(vector, other.vector);
		}

		float32_4& operator-=(const float32_4& other)
		{
			vector = _mm_sub_ps(vector, other.vector);
			return *this;
		}

		float32_4 operator*(const float32_4& other) const
		{
			return _mm_mul_ps(vector, other.vector);
		}

		float32_4& operator*=(const float32_4& other)
		{
			vector = _mm_mul_ps(vector, other.vector);
			return *this;
		}

		float32_4 operator/(const float32_4& other) const
		{
			return _mm_div_ps(vector, other.vector);
		}

		float32_4& operator/=(const float32_4& other)
		{
			vector = _mm_div_ps(vector, other.vector);
			return *this;
		}


		float32_4 operator==(const float32_4& other) const
		{
			return _mm_cmpeq_ps(vector, other.vector);
		}

		float32_4 operator!=(const float32_4& other) const
		{
			return _mm_cmpneq_ps(vector, other.vector);
		}

		float32_4 operator>(const float32_4& other) const
		{
			return _mm_cmpgt_ps(vector, other.vector);
		}

		float32_4 operator>=(const float32_4& other) const
		{
			return _mm_cmpge_ps(vector, other.vector);
		}

		float32_4 operator<(const float32_4& other) const
		{
			return _mm_cmplt_ps(vector, other.vector);
		}

		float32_4 operator<=(const float32_4& other) const
		{
			return _mm_cmple_ps(vector, other.vector);
		}

		float32_4 operator&(const float32_4& other) const
		{
			return _mm_and_ps(vector, other.vector);
		}

		float32_4 operator|(const float32_4& other) const
		{
			return _mm_or_ps(vector, other.vector);
		}
	};
}