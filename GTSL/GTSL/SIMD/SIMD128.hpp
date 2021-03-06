#pragma once

#include "SIMD.hpp"
#include <immintrin.h>

//_mm_srlv_epi32, shift right by

namespace GTSL
{
	template<typename T>
	class SIMD128;

	template<>
	class SIMD128<int8>
	{
	public:
		using type = int8;
		static constexpr uint8 TypeElementsCount = 16;

		SIMD128() = default;

		explicit SIMD128(const type a) : vector(_mm_set1_epi8(a)) {}

		SIMD128(const AlignedPointer<type, 16> data) : vector(_mm_load_si128(reinterpret_cast<__m128i*>(data.Get()))) {}
		SIMD128(const UnalignedPointer<type> data) : vector(_mm_loadu_si128(reinterpret_cast<__m128i*>(data.Get()))) {}

		SIMD128(const type a, const type b, const type c, const type d, const type e, const type f, const type g, const type h, const type i, const type j, const type k, const type l, const type m, const type n, const type o, const type p) : vector(_mm_set_epi8(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)) {}

		SIMD128(const SIMD128& other) = default;

		~SIMD128() = default;

		void Set(const AlignedPointer<type, 16> data) { vector = _mm_load_si128(reinterpret_cast<__m128i*>(data.Get())); }
		void Set(const UnalignedPointer<type> data) { vector = _mm_loadu_si128(reinterpret_cast<__m128i*>(data.Get())); }

		/**
		 * \brief Sets all of this vector's components as a.
		 * \param a float to set all of this vector's components as.
		 * \return Returns a reference to itself.
		 */
		SIMD128& operator=(const type a) { vector = _mm_set1_epi8(a); return *this; }
		SIMD128& operator=(const SIMD128& other) { vector = other.vector; return *this; }

		SIMD128& operator=(const AlignedPointer<type, 16> data) { vector = _mm_load_si128(reinterpret_cast<__m128i*>(data.Get())); return *this; }
		SIMD128& operator=(const UnalignedPointer<type> data) { vector = _mm_loadu_si128(reinterpret_cast<__m128i*>(data.Get())); return *this; }

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into aligned memory.
		void CopyTo(const AlignedPointer<type, 16> data) const { _mm_store_si128(reinterpret_cast<__m128i*>(data.Get()), vector); }

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into unaligned memory.
		void CopyTo(const UnalignedPointer<type> data) const { _mm_storeu_si128(reinterpret_cast<__m128i*>(data.Get()), vector); }

		//Shuffle single-precision (32-bit) floating-point elements in a using the control in imm8, and store the results in dst.
		template<uint8 A, uint8 B, uint8 C, uint8 D, uint8 E, uint8 F, uint8 G, uint8 H, uint8 I, uint8 J, uint8 K, uint8 L, uint8 M, uint8 N, uint8 O, uint8 P>
		[[nodiscard]] static SIMD128 Shuffle(const SIMD128& a, const SIMD128& b) { return _mm_shuffle_epi8(a.vector, SIMD128(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P)); }

		void Abs() { vector = _mm_abs_epi8(vector); }

		static SIMD128 Min(const SIMD128& a, const SIMD128& b) { return _mm_min_epi8(a, b); }
		static SIMD128 Max(const SIMD128& a, const SIMD128& b) { return _mm_max_epi8(a, b); }

		//static SIMD128 HorizontalAdd(const SIMD128& a, const SIMD128& b) { return _mm_hadd_epi8(a.vector, b.vector); }
		//
		////Horizontally add adjacent pairs of single - precision(32 - bit) floating - point elements in a and B, and pack the results in dst.
		//static SIMD128 HorizontalSub(const SIMD128& a, const SIMD128& b) { return _mm_hsub_epi8(a.vector, b.vector); }
		//
		////Alternatively add and subtract packed single-precision (32-bit) floating-point elements in a to/from packed elements in B, and store the results in dst
		//[[nodiscard]] static SIMD128 Add13Sub02(const SIMD128& a, const SIMD128& b) { return _mm_addsub_epi8(a.vector, b.vector); }
		//
		////Conditionally multiply the packed single-precision (32-bit) floating-point elements in a and B using the high 4 bits in imm8, sum the four products, and conditionally store the sum in dst using the low 4 bits of imm8.
		//[[nodiscard]] static SIMD128 DotProduct(const SIMD128& a, const SIMD128& b) { return _mm_dp_epi8(a.vector, b.vector, 0xff); }

		//static void Transpose(SIMD128& a, SIMD128& b, SIMD128& c, SIMD128& d) { _MM_TRANSPOSE4_epi8(a, b, c, d); }

		template<uint8 I>
		[[nodiscard]] type GetElement() const { return _mm_extract_epi8(vector, I); }

		SIMD128 operator+(const SIMD128& other) const { return _mm_add_epi8(vector, other.vector); }
		SIMD128 operator-(const SIMD128& other) const { return _mm_sub_epi8(vector, other.vector); }
		//SIMD128 operator*(const SIMD128& other) const { return _mm_mul_epi8(vector, other.vector); }
		SIMD128 operator/(const SIMD128& other) const { return _mm_div_epi8(vector, other.vector); }

		SIMD128& operator+=(const SIMD128& other) { vector = _mm_add_epi8(vector, other.vector); return *this; }
		SIMD128& operator-=(const SIMD128& other) { vector = _mm_sub_epi8(vector, other.vector); return *this; }
		//SIMD128& operator*=(const SIMD128& other) { vector = _mm_mul_epi8(vector, other.vector); return *this; }
		SIMD128& operator/=(const SIMD128& other) { vector = _mm_div_epi8(vector, other.vector); return *this; }

		SIMD128 operator==(const SIMD128& other) const { return _mm_cmpeq_epi8(vector, other.vector); }
		//SIMD128 operator!=(const SIMD128& other) const { return _mm_cmpneq_(vector, other.vector); }
		SIMD128 operator>(const SIMD128& other)  const { return _mm_cmpgt_epi8(vector, other.vector); }
		//SIMD128 operator>=(const SIMD128& other) const { return _mm_cmpge_si128(vector, other.vector); }
		SIMD128 operator<(const SIMD128& other)  const { return _mm_cmplt_epi8(vector, other.vector); }
		//SIMD128 operator<=(const SIMD128& other) const { return _mm_cmple_ss(vector, other.vector); }

		SIMD128 operator&(const SIMD128& other) const { return _mm_and_si128(vector, other.vector); }
		SIMD128 operator|(const SIMD128& other) const { return _mm_or_si128(vector, other.vector); }
		SIMD128 operator^(const SIMD128& other) const { return _mm_xor_si128(vector, other); }
		SIMD128& operator~() { vector = _mm_xor_si128(vector, _mm_cmpeq_epi8(vector, vector)); return *this; }

	private:
		__m128i vector;

		SIMD128(const __m128i& m128) : vector(m128) {}

		operator __m128i() const { return vector; }
	};
	
	template<>
	class SIMD128<uint8>
	{
	public:
		using type = uint8;
		static constexpr uint8 TypeElementsCount = 16;

		SIMD128() = default;

		explicit SIMD128(const type a) : vector(_mm_set1_epi8(a)) {}

		SIMD128(const AlignedPointer<type, 16> data) : vector(_mm_load_si128(reinterpret_cast<__m128i*>(data.Get()))) {}
		SIMD128(const UnalignedPointer<type> data) : vector(_mm_loadu_si128(reinterpret_cast<__m128i*>(data.Get()))) {}

		SIMD128(const type a, const type b, const type c, const type d, const type e, const type f, const type g, const type h, const type i, const type j, const type k, const type l, const type m, const type n, const type o, const type p) : vector(_mm_set_epi8(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)) {}

		SIMD128(const SIMD128& other) = default;

		~SIMD128() = default;

		void Set(const AlignedPointer<type, 16> data) { vector = _mm_load_si128(reinterpret_cast<__m128i*>(data.Get())); }
		void Set(const UnalignedPointer<type> data) { vector = _mm_loadu_si128(reinterpret_cast<__m128i*>(data.Get())); }

		/**
		 * \brief Sets all of this vector's components as a.
		 * \param a float to set all of this vector's components as.
		 * \return Returns a reference to itself.
		 */
		SIMD128& operator=(const type a) { vector = _mm_set1_epi8(a); return *this; }
		SIMD128& operator=(const SIMD128& other) { vector = other.vector; return *this; }

		SIMD128& operator=(const AlignedPointer<type, 16> data) { vector = _mm_load_si128(reinterpret_cast<__m128i*>(data.Get())); return *this; }
		SIMD128& operator=(const UnalignedPointer<type> data) { vector = _mm_loadu_si128(reinterpret_cast<__m128i*>(data.Get())); return *this; }

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into aligned memory.
		void CopyTo(const AlignedPointer<type, 16> data) const { _mm_store_si128(reinterpret_cast<__m128i*>(data.Get()), vector); }

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into unaligned memory.
		void CopyTo(const UnalignedPointer<type> data) const { _mm_storeu_si128(reinterpret_cast<__m128i*>(data.Get()), vector); }

		//Shuffle single-precision (32-bit) floating-point elements in a using the control in imm8, and store the results in dst.
		template<uint8 A, uint8 B, uint8 C, uint8 D, uint8 E, uint8 F, uint8 G, uint8 H, uint8 I, uint8 J, uint8 K, uint8 L, uint8 M, uint8 N, uint8 O, uint8 P>
		[[nodiscard]] static SIMD128 Shuffle(const SIMD128& a) { return _mm_shuffle_epi8(a.vector, SIMD128(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P)); }

		void Abs() { vector = _mm_abs_epi8(vector); }

		static SIMD128 Min(const SIMD128& a, const SIMD128& b) { return _mm_min_epi8(a, b); }
		static SIMD128 Max(const SIMD128& a, const SIMD128& b) { return _mm_max_epi8(a, b); }

		//static SIMD128 HorizontalAdd(const SIMD128& a, const SIMD128& b) { return _mm_hadd_epi8(a.vector, b.vector); }
		//
		////Horizontally add adjacent pairs of single - precision(32 - bit) floating - point elements in a and B, and pack the results in dst.
		//static SIMD128 HorizontalSub(const SIMD128& a, const SIMD128& b) { return _mm_hsub_epi8(a.vector, b.vector); }
		//
		////Alternatively add and subtract packed single-precision (32-bit) floating-point elements in a to/from packed elements in B, and store the results in dst
		//[[nodiscard]] static SIMD128 Add13Sub02(const SIMD128& a, const SIMD128& b) { return _mm_addsub_epi8(a.vector, b.vector); }
		//
		////Conditionally multiply the packed single-precision (32-bit) floating-point elements in a and B using the high 4 bits in imm8, sum the four products, and conditionally store the sum in dst using the low 4 bits of imm8.
		//[[nodiscard]] static SIMD128 DotProduct(const SIMD128& a, const SIMD128& b) { return _mm_dp_epi8(a.vector, b.vector, 0xff); }

		//static void Transpose(SIMD128& a, SIMD128& b, SIMD128& c, SIMD128& d) { _MM_TRANSPOSE4_epi8(a, b, c, d); }

		template<uint8 I>
		[[nodiscard]] type GetElement() const { return _mm_extract_epi8(vector, I); }

		SIMD128 operator+(const SIMD128& other) const { return _mm_add_epi8(vector, other.vector); }
		SIMD128 operator-(const SIMD128& other) const { return _mm_sub_epi8(vector, other.vector); }
		//SIMD128 operator*(const SIMD128& other) const { return _mm_mul_epi8(vector, other.vector); }
		SIMD128 operator/(const SIMD128& other) const { return _mm_div_epi8(vector, other.vector); }

		SIMD128& operator+=(const SIMD128& other) { vector = _mm_add_epi8(vector, other.vector); return *this; }
		SIMD128& operator-=(const SIMD128& other) { vector = _mm_sub_epi8(vector, other.vector); return *this; }
		//SIMD128& operator*=(const SIMD128& other) { vector = _mm_mul_epi8(vector, other.vector); return *this; }
		SIMD128& operator/=(const SIMD128& other) { vector = _mm_div_epi8(vector, other.vector); return *this; }

		SIMD128 operator==(const SIMD128& other) const { return _mm_cmpeq_epi8(vector, other.vector); }
		SIMD128 operator!=(const SIMD128& other) const { return _mm_andnot_si128(_mm_cmpeq_epi64(vector, other.vector), _mm_set1_epi64x(-1)); }
		SIMD128 operator>(const SIMD128& other)  const { return _mm_cmpgt_epi8(vector, other.vector); }
		SIMD128 operator>=(const SIMD128& other) const { return _mm_cmpeq_epi64(_mm_max_epu64(vector, other.vector), vector); }
		SIMD128 operator<(const SIMD128& other)  const { return _mm_cmplt_epi8(vector, other.vector); }
		SIMD128 operator<=(const SIMD128& other) const { return _mm_cmpeq_epi8(_mm_min_epu8(vector, other.vector), vector); }

		SIMD128 operator&(const SIMD128& other) const { return _mm_and_si128(vector, other.vector); }
		SIMD128 operator|(const SIMD128& other) const { return _mm_or_si128(vector, other.vector); }
		SIMD128 operator^(const SIMD128& other) const { return _mm_xor_si128(vector, other); }
		SIMD128& operator~() { vector = _mm_xor_si128(vector, _mm_cmpeq_epi8(vector, vector)); return *this; }

		static constexpr uint8 Bytes = 16;
	private:
		__m128i vector;

		SIMD128(const __m128i& m128) : vector(m128) {}

		operator __m128i() const { return vector; }
	};

	template<>
	class SIMD128<uint64>
	{
	public:
		using type = uint64;
		static constexpr uint8 TypeElementsCount = 2;

		SIMD128() = default;

		explicit SIMD128(const type a) : vector(_mm_set1_epi64x(a)) {}

		SIMD128(const AlignedPointer<type, 16> data) : vector(_mm_load_si128(reinterpret_cast<__m128i*>(data.Get()))) {}
		SIMD128(const UnalignedPointer<type> data) : vector(_mm_loadu_si128(reinterpret_cast<__m128i*>(data.Get()))) {}

		SIMD128(const type a, const type b) : vector(_mm_set_epi64x(a, b)) {}

		SIMD128(const SIMD128& other) = default;

		~SIMD128() = default;

		void Set(const AlignedPointer<type, 16> data) { vector = _mm_load_si128(reinterpret_cast<__m128i*>(data.Get())); }
		void Set(const UnalignedPointer<type> data) { vector = _mm_loadu_si128(reinterpret_cast<__m128i*>(data.Get())); }

		/**
		 * \brief Sets all of this vector's components as a.
		 * \param a float to set all of this vector's components as.
		 * \return Returns a reference to itself.
		 */
		SIMD128& operator=(const type a) { vector = _mm_set1_epi64x(a); return *this; }
		SIMD128& operator=(const SIMD128& other) { vector = other.vector; return *this; }

		SIMD128& operator=(const AlignedPointer<type, 16> data) { vector = _mm_load_si128(reinterpret_cast<__m128i*>(data.Get())); return *this; }
		SIMD128& operator=(const UnalignedPointer<type> data) { vector = _mm_loadu_si128(reinterpret_cast<__m128i*>(data.Get())); return *this; }

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into aligned memory.
		void CopyTo(const AlignedPointer<type, 16> data) const { _mm_store_si128(reinterpret_cast<__m128i*>(data.Get()), vector); }

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into unaligned memory.
		void CopyTo(const UnalignedPointer<type> data) const { _mm_storeu_si128(reinterpret_cast<__m128i*>(data.Get()), vector); }

		//Shuffle single-precision (32-bit) floating-point elements in a using the control in imm8, and store the results in dst.
		//template<uint8 A, uint8 B, uint8 C, uint8 D, uint8 E, uint8 F, uint8 G, uint8 H, uint8 I, uint8 J, uint8 K, uint8 L, uint8 M, uint8 N, uint8 O, uint8 P>
		//[[nodiscard]] static SIMD128 Shuffle(const SIMD128& a, const SIMD128& b) { return _MM_SHUFFLE2(a.vector, SIMD128(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P)); }

		void Abs() { vector = _mm_abs_epi64(vector); }

		static SIMD128 Min(const SIMD128& a, const SIMD128& b) { return _mm_min_epi64(a, b); }
		static SIMD128 Max(const SIMD128& a, const SIMD128& b) { return _mm_max_epi64(a, b); }

		//static SIMD128 HorizontalAdd(const SIMD128& a, const SIMD128& b) { return _mm_hadd_epi64(a.vector, b.vector); }
		//
		////Horizontally add adjacent pairs of single - precision(32 - bit) floating - point elements in a and B, and pack the results in dst.
		//static SIMD128 HorizontalSub(const SIMD128& a, const SIMD128& b) { return _mm_hsub_epi64(a.vector, b.vector); }
		//
		////Alternatively add and subtract packed single-precision (32-bit) floating-point elements in a to/from packed elements in B, and store the results in dst
		//[[nodiscard]] static SIMD128 Add13Sub02(const SIMD128& a, const SIMD128& b) { return _mm_addsub_epi64(a.vector, b.vector); }
		//
		////Conditionally multiply the packed single-precision (32-bit) floating-point elements in a and B using the high 4 bits in imm8, sum the four products, and conditionally store the sum in dst using the low 4 bits of imm8.
		//[[nodiscard]] static SIMD128 DotProduct(const SIMD128& a, const SIMD128& b) { return _mm_dp_epi64(a.vector, b.vector, 0xff); }

		//static void Transpose(SIMD128& a, SIMD128& b, SIMD128& c, SIMD128& d) { _MM_TRANSPOSE4_epi64(a, b, c, d); }

		template<uint8 I>
		[[nodiscard]] type GetElement() const { return _mm_extract_epi64(vector, I); }

		SIMD128 operator+(const SIMD128& other) const { return _mm_add_epi64(vector, other.vector); }
		SIMD128 operator-(const SIMD128& other) const { return _mm_sub_epi64(vector, other.vector); }
		//SIMD128 operator*(const SIMD128& other) const { return _mm_mul_epi64(vector, other.vector); }
		SIMD128 operator/(const SIMD128& other) const { return _mm_div_epi64(vector, other.vector); }

		SIMD128& operator+=(const SIMD128& other) { vector = _mm_add_epi64(vector, other.vector); return *this; }
		SIMD128& operator-=(const SIMD128& other) { vector = _mm_sub_epi64(vector, other.vector); return *this; }
		//SIMD128& operator*=(const SIMD128& other) { vector = _mm_mul_epi64(vector, other.vector); return *this; }
		SIMD128& operator/=(const SIMD128& other) { vector = _mm_div_epi64(vector, other.vector); return *this; }

		SIMD128 operator==(const SIMD128& other) const { return _mm_cmpeq_epi64(vector, other.vector); }
		SIMD128 operator!=(const SIMD128& other) const { return _mm_andnot_si128(_mm_cmpeq_epi64(vector, other.vector), _mm_set1_epi64x(-1)); }
		SIMD128 operator>(const SIMD128& other)  const { return _mm_cmpgt_epi64(vector, other.vector); }
		SIMD128 operator>=(const SIMD128& other) const { return _mm_cmpeq_epi64(_mm_max_epu64(vector, other.vector), vector); }
		SIMD128 operator<(const SIMD128& other)  const { return _mm_andnot_si128(_mm_cmpeq_epi64(_mm_max_epu64(vector, other.vector), vector), _mm_set1_epi64x(-1)); }
		SIMD128 operator<=(const SIMD128& other) const { return _mm_cmpeq_epi8(_mm_min_epu8(vector, other.vector), vector); }

		SIMD128 operator&(const SIMD128& other) const { return _mm_and_si128(vector, other.vector); }
		SIMD128 operator|(const SIMD128& other) const { return _mm_or_si128(vector, other.vector); }
		SIMD128 operator^(const SIMD128& other) const { return _mm_xor_si128(vector, other); }
		SIMD128& operator~() { vector = _mm_xor_si128(vector, _mm_cmpeq_epi64(vector, vector)); return *this; }
	private:
		__m128i vector;

		SIMD128(const __m128i& m128) : vector(m128) {}

		operator __m128i() const { return vector; }
	};

	template<>
	class SIMD128<int32>;
	
	template<>
	class SIMD128<float32>
	{
	public:
		using type = float32;
		static constexpr uint8 TypeElementsCount = 4;
		
		SIMD128() = default;

		explicit SIMD128(const type a) : vector(_mm_set_ps1(a)) {}

		SIMD128(const AlignedPointer<const type, 16> data) : vector(_mm_load_ps(data)) {}
		SIMD128(const UnalignedPointer<const type> data) : vector(_mm_loadu_ps(data)) {}

		/**
		 * \brief Loads elements in to the vector, it stores in order from left to right, doen't use SSE native little-endian ordering
		 * \param x 0th parameter in vector
		 * \param y 1st parameter in vector
		 * \param z 2nd parameter in vector
		 * \param w 3rd parameter in vector
		 */
		SIMD128(const type x, const type y, const type z, const type w) : vector(_mm_setr_ps(x, y, z, w)) {}
		explicit SIMD128(const SIMD128<int32> other);
		
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

		SIMD128& operator=(const AlignedPointer<const type, 16> data) { vector = _mm_load_ps(data); return *this; }
		SIMD128& operator=(const UnalignedPointer<const type> data) { vector = _mm_loadu_ps(data); return *this; }

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into aligned memory.
		void CopyTo(const AlignedPointer<type, 16> data) const { _mm_store_ps(data, vector); }

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into unaligned memory.
		void CopyTo(const UnalignedPointer<type> data) const { _mm_storeu_ps(data, vector); }

		//Shuffle single-precision (32-bit) floating-point elements in a using the control in imm8, and store the results in dst.
		template<int32 A, int32 B, int32 C, int32 D>
		[[nodiscard]] static SIMD128 Shuffle(const SIMD128 & a, const SIMD128 & b) { return _mm_shuffle_ps(a.vector, b.vector, _MM_SHUFFLE(A, B, C, D)); }

		void Abs() { vector = _mm_andnot_ps(vector, SIMD128(1.0f)); }
		static SIMD128 Abs(const SIMD128& a) { return _mm_andnot_ps(a, SIMD128(-0.0f)); }
		static SIMD128 NotAbs(const SIMD128& a) { return _mm_andnot_ps(a, SIMD128(0.0f)); }

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

		SIMD128(const __m128 m128) : vector(m128) {}

		operator __m128() const { return vector; }

		friend class SIMD128<int32>;
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
		SIMD128& operator=(const float a) { vector = _mm_set1_pd(a); return *this; }
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

	template<>
	class SIMD128<int32>
	{
	public:
		using type = int32;
		static constexpr uint8 TypeElementsCount = 4;

		SIMD128() = default;

		explicit SIMD128(const type a) : vector(_mm_set1_epi32(a)) {}

		SIMD128(const AlignedPointer<type, 16> data) : vector(_mm_load_si128(reinterpret_cast<__m128i*>(data.Get()))) {}
		SIMD128(const UnalignedPointer<type> data) : vector(_mm_loadu_si128(reinterpret_cast<__m128i*>(data.Get()))) {}

		SIMD128(const SIMD128<float32> other) : vector(_mm_castps_si128(other.vector)) {}
		
		SIMD128(const type a, const type b, const type c, const type d) : vector(_mm_set_epi32(a, b, c, d)) {}

		SIMD128(const SIMD128& other) = default;

		~SIMD128() = default;

		void Set(const AlignedPointer<type, 16> data) { vector = _mm_load_si128(reinterpret_cast<__m128i*>(data.Get())); }
		void Set(const UnalignedPointer<type> data) { vector = _mm_loadu_si128(reinterpret_cast<__m128i*>(data.Get())); }

		/**
		 * \brief Sets all of this vector's components as a.
		 * \param a float to set all of this vector's components as.
		 * \return Returns a reference to itself.
		 */
		SIMD128& operator=(const type a) { vector = _mm_set1_epi64x(a); return *this; }
		SIMD128& operator=(const SIMD128& other) { vector = other.vector; return *this; }

		SIMD128& operator=(const AlignedPointer<type, 16> data) { vector = _mm_load_si128(reinterpret_cast<__m128i*>(data.Get())); return *this; }
		SIMD128& operator=(const UnalignedPointer<type> data) { vector = _mm_loadu_si128(reinterpret_cast<__m128i*>(data.Get())); return *this; }

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into aligned memory.
		void CopyTo(const AlignedPointer<type, 16> data) const { _mm_store_si128(reinterpret_cast<__m128i*>(data.Get()), vector); }

		//Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this vector into unaligned memory.
		void CopyTo(const UnalignedPointer<type> data) const { _mm_storeu_si128(reinterpret_cast<__m128i*>(data.Get()), vector); }

		//Shuffle single-precision (32-bit) floating-point elements in a using the control in imm8, and store the results in dst.
		//template<uint8 A, uint8 B, uint8 C, uint8 D, uint8 E, uint8 F, uint8 G, uint8 H, uint8 I, uint8 J, uint8 K, uint8 L, uint8 M, uint8 N, uint8 O, uint8 P>
		//[[nodiscard]] static SIMD128 Shuffle(const SIMD128& a, const SIMD128& b) { return _MM_SHUFFLE2(a.vector, SIMD128(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P)); }

		void Abs() { vector = _mm_abs_epi32(vector); }

		static SIMD128 Min(const SIMD128& a, const SIMD128& b) { return _mm_min_epi32(a, b); }
		static SIMD128 Max(const SIMD128& a, const SIMD128& b) { return _mm_max_epi32(a, b); }

		template<uint8 I>
		[[nodiscard]] type GetElement() const { return _mm_extract_epi32(vector, I); }

		SIMD128 operator+(const SIMD128& other) const { return _mm_add_epi32(vector, other.vector); }
		SIMD128 operator-(const SIMD128& other) const { return _mm_sub_epi32(vector, other.vector); }
		SIMD128 operator*(const SIMD128& other) const { return _mm_mul_epi32(vector, other.vector); }
		SIMD128 operator/(const SIMD128& other) const { return _mm_div_epi32(vector, other.vector); }

		SIMD128& operator+=(const SIMD128& other) { vector = _mm_add_epi32(vector, other.vector); return *this; }
		SIMD128& operator-=(const SIMD128& other) { vector = _mm_sub_epi32(vector, other.vector); return *this; }
		SIMD128& operator*=(const SIMD128& other) { vector = _mm_mul_epi32(vector, other.vector); return *this; }
		SIMD128& operator/=(const SIMD128& other) { vector = _mm_div_epi32(vector, other.vector); return *this; }

		SIMD128 operator==(const SIMD128& other) const { return _mm_cmpeq_epi32(vector, other.vector); }
		SIMD128 operator!=(const SIMD128& other) const { return _mm_andnot_si128(_mm_cmpeq_epi32(vector, other.vector), _mm_set1_epi32(-1)); }
		SIMD128 operator>(const SIMD128& other)  const { return _mm_cmpgt_epi32(vector, other.vector); }
		SIMD128 operator>=(const SIMD128& other) const { return _mm_cmpeq_epi32(_mm_max_epi32(vector, other.vector), vector); }
		SIMD128 operator<(const SIMD128& other)  const { return _mm_andnot_si128(_mm_cmpeq_epi32(_mm_max_epi32(vector, other.vector), vector), _mm_set1_epi32(-1)); }
		SIMD128 operator<=(const SIMD128& other) const { return _mm_cmpeq_epi32(_mm_min_epi32(vector, other.vector), vector); }

		SIMD128 operator&(const SIMD128& other) const { return _mm_and_si128(vector, other.vector); }
		SIMD128 operator|(const SIMD128& other) const { return _mm_or_si128(vector, other.vector); }
		SIMD128 operator^(const SIMD128& other) const { return _mm_xor_si128(vector, other); }
		SIMD128& operator~() { vector = _mm_xor_si128(vector, _mm_cmpeq_epi32(vector, vector)); return *this; }
	private:
		__m128i vector;

		SIMD128(const __m128i& m128) : vector(m128) {}

		operator __m128i() const { return vector; }

		friend class SIMD128<float32>;
	};

	inline SIMD128<float32>::SIMD128(const SIMD128<int32> other) : vector(_mm_castsi128_ps(other.vector)) {}
}
