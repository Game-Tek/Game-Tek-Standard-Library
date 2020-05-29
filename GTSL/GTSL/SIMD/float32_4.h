#pragma once

#include <immintrin.h>

#include "SIMD.h"

//_mm_srlv_epi32, shift right by

namespace GTSL
{
	class float32_4
	{
		__m128 vector;

		float32_4(const __m128& m128) : vector(m128)
		{
		}

	public:
		float32_4() : vector(_mm_setzero_ps())
		{
		}

		explicit float32_4(const float a) : vector(_mm_set_ps1(a))
		{
		}

		float32_4(const AlignedPointer<float, 16> data) : vector(_mm_load_ps(data))
		{
		}

		float32_4(const UnalignedPointer<float> data) : vector(_mm_loadu_ps(data))
		{
		}
		
		float32_4(const float x, const float y, const float z, const float w) : vector(_mm_set_ps(w, z, y, x))
		{
		}

		~float32_4() = default;

		operator __m128() const { return vector; }

		void Set(const AlignedPointer<float, 16> data) { vector = _mm_load_ps(data); }
		void Set(const UnalignedPointer<float> data) { vector = _mm_loadu_ps(data); }

		/**
		 * \brief Sets all of this vector's components as a.
		 * \param a float to set all of this vector's components as.
		 * \return Returns a reference to itself.
		 */
		float32_4& operator=(const float a) { vector = _mm_set_ps1(a); return *this; }

		float32_4& operator=(const AlignedPointer<float, 16> data) { vector = _mm_load_ps(data); return *this; }
		float32_4& operator=(const UnalignedPointer<float> data)   { vector = _mm_loadu_ps(data); return *this; }
		
		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into aligned memory.
		void CopyTo(const AlignedPointer<float, 16> data) const { _mm_store_ps(data, vector); }
		
		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into unaligned memory.
		void CopyTo(const UnalignedPointer<float> data) const { _mm_storeu_ps(data, vector); }

		//Shuffle single-precision (32-bit) floating-point elements in a using the control in imm8, and store the results in dst.
		template<int32 A, int32 B, int32 C, int32 D>
		[[nodiscard]] static float32_4 Shuffle(const float32_4& a, const float32_4& b) { return _mm_shuffle_ps(a.vector, b.vector, _MM_SHUFFLE(A, B, C, D)); }

		static float32_4 Abs(const float32_4& a) { return _mm_andnot_ps(a.vector, float32_4(1.0f)); }

		static float32_4 HorizontalAdd(const float32_4& a, const float32_4& b) { return _mm_hadd_ps(a.vector, b.vector); }

		//Horizontally add adjacent pairs of single - precision(32 - bit) floating - point elements in a and B, and pack the results in dst.
		[[nodiscard]] static float32_4 HorizontalSub(const float32_4& a, const float32_4& b) { return _mm_hsub_ps(a.vector, b.vector); }

		//Alternatively add and subtract packed single-precision (32-bit) floating-point elements in a to/from packed elements in B, and store the results in dst
		[[nodiscard]] static float32_4 Add13Sub02(const float32_4& a, const float32_4& b) { return _mm_addsub_ps(a.vector, b.vector); }

		//Conditionally multiply the packed single-precision (32-bit) floating-point elements in a and B using the high 4 bits in imm8, sum the four products, and conditionally store the sum in dst using the low 4 bits of imm8.
		[[nodiscard]] static float32_4 DotProduct(const float32_4& a, const float32_4& b) { return _mm_dp_ps(a.vector, b.vector, 0xff); }

		static void Transpose(float32_4& a, float32_4& b, float32_4& c, float32_4& d) { _MM_TRANSPOSE4_PS(a, b, c, d); }

		[[nodiscard]] float32_4 SquareRoot() const { return _mm_sqrt_ps(vector); }

		//Compute the square root of the lower single-precision (32-bit) floating-point element in a, store the result in the lower element of dst, and copy the upper 3 packed elements from a to the upper elements of dst.
		[[nodiscard]] float32_4 SquareRootToLower(const float32_4& a) const { return _mm_sqrt_ss(vector); }

		template<uint8 I>
		[[nodiscard]] float GetElement() const { return _mm_extract_ps(vector, I); }

		float32_4 operator+(const float32_4& other) const {	return _mm_add_ps(vector, other.vector); }

		float32_4& operator+=(const float32_4& other) { vector = _mm_add_ps(vector, other.vector); return *this; }

		float32_4 operator-(const float32_4& other) const { return _mm_sub_ps(vector, other.vector); }

		float32_4& operator-=(const float32_4& other) { vector = _mm_sub_ps(vector, other.vector); return *this; }

		float32_4 operator*(const float32_4& other) const { return _mm_mul_ps(vector, other.vector); }

		float32_4& operator*=(const float32_4& other) { vector = _mm_mul_ps(vector, other.vector); return *this; }

		float32_4 operator/(const float32_4& other) const { return _mm_div_ps(vector, other.vector); }

		float32_4& operator/=(const float32_4& other) { vector = _mm_div_ps(vector, other.vector); return *this; }


		float32_4 operator==(const float32_4& other) const { return _mm_cmpeq_ps(vector, other.vector); }

		float32_4 operator!=(const float32_4& other) const { return _mm_cmpneq_ps(vector, other.vector); }

		float32_4 operator>(const float32_4& other) const { return _mm_cmpgt_ps(vector, other.vector); }

		float32_4 operator>=(const float32_4& other) const { return _mm_cmpge_ps(vector, other.vector); }

		float32_4 operator<(const float32_4& other) const { return _mm_cmplt_ps(vector, other.vector); }

		float32_4 operator<=(const float32_4& other) const { return _mm_cmple_ps(vector, other.vector); }

		float32_4 operator&(const float32_4& other) const { return _mm_and_ps(vector, other.vector); }

		float32_4 operator|(const float32_4& other) const { return _mm_or_ps(vector, other.vector); }
	};
}
