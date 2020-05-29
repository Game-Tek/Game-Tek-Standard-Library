#pragma once

#include <immintrin.h>

#include "SIMD.h"

//_mm_srlv_epi32, shift right by

namespace GTSL
{
	template<typename T>
	class SIMD128;

	template<>
	class SIMD128<float32>
	{
	public:
		using type = float32;
		static constexpr uint8 TypeElementsCount = 4;
		
		SIMD128() = default;

		explicit SIMD128(const type a) : vector(_mm_set_ps1(a)) {}

		SIMD128(const AlignedPointer<type, 16> data) : vector(_mm_load_ps(data)) {}
		SIMD128(const UnalignedPointer<type> data) : vector(_mm_loadu_ps(data)) {}

		SIMD128(const type x, const type y, const type z, const type w) : vector(_mm_set_ps(x, y, z, w)) {}

		SIMD128(const SIMD128 & other) = default;

		~SIMD128() = default;

		void Set(const AlignedPointer<type, 16> data) { vector = _mm_load_ps(data); }
		void Set(const UnalignedPointer<type> data) { vector = _mm_loadu_ps(data); }

		/**
		 * \brief Sets all of this vector's components as a.
		 * \param a float to set all of this vector's components as.
		 * \return Returns a reference to itself.
		 */
		SIMD128& operator=(const type a) { vector = _mm_set_ps1(a); return *this; }
		SIMD128& operator=(const SIMD128 & other) { vector = other.vector; return *this; }

		SIMD128& operator=(const AlignedPointer<type, 16> data) { vector = _mm_load_ps(data); return *this; }
		SIMD128& operator=(const UnalignedPointer<type> data) { vector = _mm_loadu_ps(data); return *this; }

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into aligned memory.
		void CopyTo(const AlignedPointer<type, 16> data) const { _mm_store_ps(data, vector); }

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into unaligned memory.
		void CopyTo(const UnalignedPointer<type> data) const { _mm_storeu_ps(data, vector); }

		//Shuffle single-precision (32-bit) floating-point elements in a using the control in imm8, and store the results in dst.
		template<int32 A, int32 B, int32 C, int32 D>
		[[nodiscard]] static SIMD128 Shuffle(const SIMD128 & a, const SIMD128 & b) { return _mm_shuffle_ps(a.vector, b.vector, _MM_SHUFFLE(A, B, C, D)); }

		void Abs() { vector = _mm_andnot_ps(vector, SIMD128(1.0f)); }

		static SIMD128 Min(const SIMD128 & a, const SIMD128 & b) { return _mm_min_ps(a, b); }
		static SIMD128 Max(const SIMD128 & a, const SIMD128 & b) { return _mm_max_ps(a, b); }

		static SIMD128 HorizontalAdd(const SIMD128 & a, const SIMD128 & b) { return _mm_hadd_ps(a.vector, b.vector); }

		//Horizontally add adjacent pairs of single - precision(32 - bit) floating - point elements in a and B, and pack the results in dst.
		static SIMD128 HorizontalSub(const SIMD128 & a, const SIMD128 & b) { return _mm_hsub_ps(a.vector, b.vector); }

		//Alternatively add and subtract packed single-precision (32-bit) floating-point elements in a to/from packed elements in B, and store the results in dst
		[[nodiscard]] static SIMD128 Add13Sub02(const SIMD128 & a, const SIMD128 & b) { return _mm_addsub_ps(a.vector, b.vector); }

		//Conditionally multiply the packed single-precision (32-bit) floating-point elements in a and B using the high 4 bits in imm8, sum the four products, and conditionally store the sum in dst using the low 4 bits of imm8.
		[[nodiscard]] static SIMD128 DotProduct(const SIMD128 & a, const SIMD128 & b) { return _mm_dp_ps(a.vector, b.vector, 0xff); }

		static void Transpose(SIMD128 & a, SIMD128 & b, SIMD128 & c, SIMD128 & d) { _MM_TRANSPOSE4_PS(a, b, c, d); }

		[[nodiscard]] SIMD128 SquareRoot() const { return _mm_sqrt_ps(vector); }

		/**
		 * \brief Computes the square root of the lower single-precision (32-bit) floating-point element, stores the result in the lower element of the return, and copies the upper 3 elements from the vector to the upper elements of the return.
		 * \return A vector containing the square root of the vector's first element in the lowest element and the vector upper 3 elements in the upper 3 elements.
		 */
		[[nodiscard]] SIMD128 SquareRootToLower() const { return _mm_sqrt_ss(vector); }

		template<uint8 I>
		[[nodiscard]] type GetElement() const { return _mm_extract_ps(vector, I); }

		SIMD128 operator+(const SIMD128 & other) const { return _mm_add_ps(vector, other.vector); }
		SIMD128 operator-(const SIMD128 & other) const { return _mm_sub_ps(vector, other.vector); }
		SIMD128 operator*(const SIMD128 & other) const { return _mm_mul_ps(vector, other.vector); }
		SIMD128 operator/(const SIMD128 & other) const { return _mm_div_ps(vector, other.vector); }

		SIMD128& operator+=(const SIMD128 & other) { vector = _mm_add_ps(vector, other.vector); return *this; }
		SIMD128& operator-=(const SIMD128 & other) { vector = _mm_sub_ps(vector, other.vector); return *this; }
		SIMD128& operator*=(const SIMD128 & other) { vector = _mm_mul_ps(vector, other.vector); return *this; }
		SIMD128& operator/=(const SIMD128 & other) { vector = _mm_div_ps(vector, other.vector); return *this; }

		SIMD128 operator==(const SIMD128 & other) const { return _mm_cmpeq_ps(vector, other.vector); }
		SIMD128 operator!=(const SIMD128 & other) const { return _mm_cmpneq_ps(vector, other.vector); }
		SIMD128 operator>(const SIMD128 & other)  const { return _mm_cmpgt_ps(vector, other.vector); }
		SIMD128 operator>=(const SIMD128 & other) const { return _mm_cmpge_ps(vector, other.vector); }
		SIMD128 operator<(const SIMD128 & other)  const { return _mm_cmplt_ps(vector, other.vector); }
		SIMD128 operator<=(const SIMD128 & other) const { return _mm_cmple_ps(vector, other.vector); }

		SIMD128 operator&(const SIMD128 & other) const { return _mm_and_ps(vector, other.vector); }
		SIMD128 operator|(const SIMD128 & other) const { return _mm_or_ps(vector, other.vector); }
		SIMD128 operator^(const SIMD128 & other) const { return _mm_xor_ps(vector, other); }
		SIMD128& operator~() { vector = _mm_xor_ps(vector, _mm_cmpeq_ps(vector, vector)); return *this; }

	private:
		__m128 vector;

		SIMD128(const __m128 & m128) : vector(m128) {}

		operator __m128() const { return vector; }
	};

	template<>
	class SIMD128<float64>
	{
	public:
		using type = float64;
		static constexpr uint8 TypeElementsCount = 2;
		
		SIMD128() = default;

		explicit SIMD128(const type a) : vector(_mm_set_pd1(a)) {}

		SIMD128(const AlignedPointer<type, 16> data) : vector(_mm_load_pd(data)) {}
		SIMD128(const UnalignedPointer<type> data) : vector(_mm_loadu_pd(data)) {}

		SIMD128(const type x, const type y) : vector(_mm_set_pd(x, y)) {}

		SIMD128(const SIMD128& other) = default;

		~SIMD128() = default;

		void Set(const AlignedPointer<type, 16> data) { vector = _mm_load_pd(data); }
		void Set(const UnalignedPointer<type> data) { vector = _mm_loadu_pd(data); }

		/**
		 * \brief Sets all of this vector's components as a.
		 * \param a float to set all of this vector's components as.
		 * \return Returns a reference to itself.
		 */
		SIMD128& operator=(const float a) { vector = _mm_set_pd1(a); return *this; }
		SIMD128& operator=(const SIMD128& other) { vector = other.vector; return *this; }

		SIMD128& operator=(const AlignedPointer<type, 16> data) { vector = _mm_load_pd(data); return *this; }
		SIMD128& operator=(const UnalignedPointer<type> data) { vector = _mm_loadu_pd(data); return *this; }

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into aligned memory.
		void CopyTo(const AlignedPointer<type, 16> data) const { _mm_store_pd(data, vector); }

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into unaligned memory.
		void CopyTo(const UnalignedPointer<type> data) const { _mm_storeu_pd(data, vector); }

		//Shuffle single-precision (32-bit) floating-point elements in a using the control in imm8, and store the results in dst.
		template<int32 A, int32 B, int32 C, int32 D>
		[[nodiscard]] static SIMD128 Shuffle(const SIMD128& a, const SIMD128& b) { return _mm_shuffle_pd(a.vector, b.vector, _MM_SHUFFLE(A, B, C, D)); }

		void Abs() { vector = _mm_andnot_pd(vector, SIMD128(1.0f)); }

		static SIMD128 Min(const SIMD128& a, const SIMD128& b) { return _mm_min_pd(a, b); }
		static SIMD128 Max(const SIMD128& a, const SIMD128& b) { return _mm_max_pd(a, b); }

		static SIMD128 HorizontalAdd(const SIMD128& a, const SIMD128& b) { return _mm_hadd_pd(a.vector, b.vector); }

		//Horizontally add adjacent pairs of single - precision(32 - bit) floating - point elements in a and B, and pack the results in dst.
		static SIMD128 HorizontalSub(const SIMD128& a, const SIMD128& b) { return _mm_hsub_pd(a.vector, b.vector); }

		//Alternatively add and subtract packed single-precision (32-bit) floating-point elements in a to/from packed elements in B, and store the results in dst
		[[nodiscard]] static SIMD128 Add13Sub02(const SIMD128& a, const SIMD128& b) { return _mm_addsub_pd(a.vector, b.vector); }

		//Conditionally multiply the packed single-precision (32-bit) floating-point elements in a and B using the high 4 bits in imm8, sum the four products, and conditionally store the sum in dst using the low 4 bits of imm8.
		[[nodiscard]] static SIMD128 DotProduct(const SIMD128& a, const SIMD128& b) { return _mm_dp_pd(a.vector, b.vector, 0xff); }

		//static void Transpose(SIMD128& a, SIMD128& b, SIMD128& c, SIMD128& d) { _MM_TRANSPOSE4_PD(a, b, c, d); }

		[[nodiscard]] SIMD128 SquareRoot() const { return _mm_sqrt_pd(vector); }

		/**
		 * \brief Computes the square root of the lower single-precision (32-bit) floating-point element, stores the result in the lower element of the return, and copies the upper 3 elements from the vector to the upper elements of the return.
		 * \return A vector containing the square root of the vector's first element in the lowest element and the vector upper 3 elements in the upper 3 elements.
		 */
		[[nodiscard]] SIMD128 SquareRootToLower() const { return _mm_sqrt_sd(vector, vector); }

		//http://searchivarius.org/blog/whats-wrong-mmextractps-and-mmextractpd
		template<uint8 I>
		[[nodiscard]] float GetElement() const { return _mm_cvtsd_f64(_mm_shuffle_pd(vector, vector, _MM_SHUFFLE2(0, I))); }

		SIMD128 operator+(const SIMD128& other) const { return _mm_add_pd(vector, other.vector); }
		SIMD128 operator-(const SIMD128& other) const { return _mm_sub_pd(vector, other.vector); }
		SIMD128 operator*(const SIMD128& other) const { return _mm_mul_pd(vector, other.vector); }
		SIMD128 operator/(const SIMD128& other) const { return _mm_div_pd(vector, other.vector); }

		SIMD128& operator+=(const SIMD128& other) { vector = _mm_add_pd(vector, other.vector); return *this; }
		SIMD128& operator-=(const SIMD128& other) { vector = _mm_sub_pd(vector, other.vector); return *this; }
		SIMD128& operator*=(const SIMD128& other) { vector = _mm_mul_pd(vector, other.vector); return *this; }
		SIMD128& operator/=(const SIMD128& other) { vector = _mm_div_pd(vector, other.vector); return *this; }

		SIMD128 operator==(const SIMD128& other) const { return _mm_cmpeq_pd(vector, other.vector); }
		SIMD128 operator!=(const SIMD128& other) const { return _mm_cmpneq_pd(vector, other.vector); }
		SIMD128 operator>(const SIMD128& other)  const { return _mm_cmpgt_pd(vector, other.vector); }
		SIMD128 operator>=(const SIMD128& other) const { return _mm_cmpge_pd(vector, other.vector); }
		SIMD128 operator<(const SIMD128& other)  const { return _mm_cmplt_pd(vector, other.vector); }
		SIMD128 operator<=(const SIMD128& other) const { return _mm_cmple_pd(vector, other.vector); }

		SIMD128 operator&(const SIMD128& other) const { return _mm_and_pd(vector, other.vector); }
		SIMD128 operator|(const SIMD128& other) const { return _mm_or_pd(vector, other.vector); }
		SIMD128 operator^(const SIMD128& other) const { return _mm_xor_pd(vector, other); }
		SIMD128& operator~() { vector = _mm_xor_pd(vector, _mm_cmpeq_pd(vector, vector)); return *this; }

	private:
		__m128d vector;

		SIMD128(const __m128d& m128) : vector(m128) {}

		operator __m128d() const { return vector; }
	};
}
