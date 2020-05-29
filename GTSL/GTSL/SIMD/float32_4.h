#pragma once

#include <immintrin.h>

#include "SIMD.h"

//_mm_srlv_epi32, shift right by

namespace GTSL
{
	class float32_4
	{
	public:
		float32_4() = default;

		explicit float32_4(const float a) : vector(_mm_set_ps1(a)) {}

		float32_4(const AlignedPointer<float, 16> data) : vector(_mm_load_ps(data)) {}
		float32_4(const UnalignedPointer<float> data) : vector(_mm_loadu_ps(data)) {}
		
		float32_4(const float x, const float y, const float z, const float w) : vector(_mm_set_ps(x, y, z, w)) {}

		float32_4(const float32_4& float324) = default;
		
		~float32_4() = default;

		void Set(const AlignedPointer<float, 16> data) { vector = _mm_load_ps(data); }
		void Set(const UnalignedPointer<float> data) { vector = _mm_loadu_ps(data); }

		/**
		 * \brief Sets all of this vector's components as a.
		 * \param a float to set all of this vector's components as.
		 * \return Returns a reference to itself.
		 */
		float32_4& operator=(const float a) { vector = _mm_set_ps1(a); return *this; }
		float32_4& operator=(const float32_4& other) { vector = other.vector; return *this; }

		float32_4& operator=(const AlignedPointer<float, 16> data) { vector = _mm_load_ps(data); return *this; }
		float32_4& operator=(const UnalignedPointer<float> data)   { vector = _mm_loadu_ps(data); return *this; }
		
		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into aligned memory.
		void CopyTo(const AlignedPointer<float, 16> data) const { _mm_store_ps(data, vector); }
		
		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into unaligned memory.
		void CopyTo(const UnalignedPointer<float> data) const { _mm_storeu_ps(data, vector); }

		//Shuffle single-precision (32-bit) floating-point elements in a using the control in imm8, and store the results in dst.
		template<int32 A, int32 B, int32 C, int32 D>
		[[nodiscard]] static float32_4 Shuffle(const float32_4& a, const float32_4& b) { return _mm_shuffle_ps(a.vector, b.vector, _MM_SHUFFLE(A, B, C, D)); }

		void Abs() { vector = _mm_andnot_ps(vector, float32_4(1.0f)); }

		static float32_4 Min(const float32_4& a, const float32_4& b) { return _mm_min_ps(a, b); }
		static float32_4 Max(const float32_4& a, const float32_4& b) { return _mm_max_ps(a, b); }
		
		static float32_4 HorizontalAdd(const float32_4& a, const float32_4& b) { return _mm_hadd_ps(a.vector, b.vector); }

		//Horizontally add adjacent pairs of single - precision(32 - bit) floating - point elements in a and B, and pack the results in dst.
		static float32_4 HorizontalSub(const float32_4& a, const float32_4& b) { return _mm_hsub_ps(a.vector, b.vector); }

		//Alternatively add and subtract packed single-precision (32-bit) floating-point elements in a to/from packed elements in B, and store the results in dst
		[[nodiscard]] static float32_4 Add13Sub02(const float32_4& a, const float32_4& b) { return _mm_addsub_ps(a.vector, b.vector); }

		//Conditionally multiply the packed single-precision (32-bit) floating-point elements in a and B using the high 4 bits in imm8, sum the four products, and conditionally store the sum in dst using the low 4 bits of imm8.
		[[nodiscard]] static float32_4 DotProduct(const float32_4& a, const float32_4& b) { return _mm_dp_ps(a.vector, b.vector, 0xff); }

		static void Transpose(float32_4& a, float32_4& b, float32_4& c, float32_4& d) { _MM_TRANSPOSE4_PS(a, b, c, d); }

		[[nodiscard]] float32_4 SquareRoot() const { return _mm_sqrt_ps(vector); }

		/**
		 * \brief Computes the square root of the lower single-precision (32-bit) floating-point element, stores the result in the lower element of the return, and copies the upper 3 elements from the vector to the upper elements of the return.
		 * \return A vector containing the square root of the vector's first element in the lowest element and the vector upper 3 elements in the upper 3 elements.
		 */
		[[nodiscard]] float32_4 SquareRootToLower() const { return _mm_sqrt_ss(vector); }

		template<uint8 I>
		[[nodiscard]] float GetElement() const { return _mm_extract_ps(vector, I); }

		float32_4 operator+(const float32_4& other) const {	return _mm_add_ps(vector, other.vector); }
		float32_4 operator-(const float32_4& other) const { return _mm_sub_ps(vector, other.vector); }
		float32_4 operator*(const float32_4& other) const { return _mm_mul_ps(vector, other.vector); }
		float32_4 operator/(const float32_4& other) const { return _mm_div_ps(vector, other.vector); }

		float32_4& operator+=(const float32_4& other) { vector = _mm_add_ps(vector, other.vector); return *this; }
		float32_4& operator-=(const float32_4& other) { vector = _mm_sub_ps(vector, other.vector); return *this; }
		float32_4& operator*=(const float32_4& other) { vector = _mm_mul_ps(vector, other.vector); return *this; }
		float32_4& operator/=(const float32_4& other) { vector = _mm_div_ps(vector, other.vector); return *this; }

		float32_4 operator==(const float32_4& other) const { return _mm_cmpeq_ps(vector, other.vector); }
		float32_4 operator!=(const float32_4& other) const { return _mm_cmpneq_ps(vector, other.vector); }
		float32_4 operator>(const float32_4& other)  const { return _mm_cmpgt_ps(vector, other.vector); }
		float32_4 operator>=(const float32_4& other) const { return _mm_cmpge_ps(vector, other.vector); }
		float32_4 operator<(const float32_4& other)  const { return _mm_cmplt_ps(vector, other.vector); }
		float32_4 operator<=(const float32_4& other) const { return _mm_cmple_ps(vector, other.vector); }

		float32_4 operator&(const float32_4& other) const { return _mm_and_ps(vector, other.vector); }
		float32_4 operator|(const float32_4& other) const { return _mm_or_ps(vector, other.vector); }
		float32_4 operator^(const float32_4& other) const { return _mm_xor_ps(vector, other); }
		float32_4& operator~() { vector = _mm_xor_ps(vector, _mm_cmpeq_ps(vector, vector)); return *this; }

	private:
		__m128 vector;

		float32_4(const __m128& m128) : vector(m128) {}

		operator __m128() const { return vector; }

	};
}
