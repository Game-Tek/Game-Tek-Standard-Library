#pragma once

#include <immintrin.h>
#include <intrin0.h>

#include "AxisAngle.h"
#include "GTSL/Core.h"
#include "Vectors.h"
#include "Quaternion.h"
#include "Matrix4.h"
#include "Plane.h"
#include "Rotator.h"
#include "GTSL/Assert.h"
#include "GTSL/Range.hpp"
#include <GTSL/Extent.h>

#include "GTSL/SIMD/SIMD128.hpp"

namespace GTSL
{
	union FloatintPoint
	{
		FloatintPoint(float32 f) : Float(f) {}
		FloatintPoint(int32 i) : Int(i) {}

		float32 Float; int32 Int;
	};

	template<typename T>
	class TypedVal {
		T value;
	public:
		TypedVal(const T val) : value(val) {}
		T operator()() const { return value; }
	};
	
	template<typename T>
	class Radian : public TypedVal<T> {};

	template<typename T>
	class Degree : public TypedVal<T> {};

	using Degrees = Degree<float32>;
	using Radians = Radian<float32>;
	
	/**
	 * \brief Provides most of the same methods of math but usually skips input checking for non valid inputs, NaN, etc. Useful when working in conditions which assure you certain
	 * validity of inputs. This functions are faster than those of Math.
	 */
	namespace AdvancedMath {
		/**
		 * \brief Computes a square root via IEEE754 bit hacking which is faster but less accurate(6% deviation). Accepts only positive values, undefined for negative values or NaN.
		 * \param x Number to compute the square root of
		 * \return The square root of x
		 */
		inline float32 FastImpreciseSqrt(const float32 x) {
			return FloatintPoint((FloatintPoint(x).Int >> 1) + (FloatintPoint(1.0f).Int >> 1)).Float; // aprox
		}

		/**
		 * \brief Computes a more precise square root via IEEE754 bit hacking (0.04% deviation). Accepts only positive values, undefined for negative values or NaN.
		 * \param x Number to compute the square root of
		 * \return The square root of x
		 */
		inline float32 FastSqrt(const float32 x) {
			auto y = FastImpreciseSqrt(x);
			return (y * y + x) / (2.0f * y); //Newton-Rhapson iteration for increased precision
		}

		template<typename T>
		T SquareRoot(const T a) {
			T s = a; T aS;

			const uint32 iterations = static_cast<uint32>(a * static_cast<T>(0.008));

			auto loop = [&]() { aS = a / s;	s = (s + aS) * static_cast<T>(0.5); };

			loop(); loop(); loop(); //3 fixed iterations
			
			for(uint32 i = 0; i < iterations; ++i) { loop(); } //variable iterations for extra precision on larger numbers

			return s;
		}

		inline float32 Interp(const float32 target, const float32 current, const float32 factor) {
			return current + (target - current) * factor;
		}

		inline float32 InterpDelta(const float32 target, const float32 current, const float32 factor) {
			return (target - current) * factor;
		}

		inline float32 Modulo(const float32 a, const float32 b, float32& c, int32& intC) {
			c = a / b; intC = static_cast<int32>(c);
			return (c - intC) * b;
		}

		inline float32 Abs(const float32 a) { return a < 0.0f ? -a : a; }
	}

	namespace Math
	{
		struct float16
		{
			explicit float16(const float32 a) : float16(*((uint32*)&a))
			{
			}

			explicit operator float32() const
			{
				//auto tt = (Sign << 16) | ((Exponent + 0x1C000) << 13) | (Fraction << 13);
				auto tt = ((halfFloat & 0x8000) << 16) | (((halfFloat & 0x7c00) + 0x1C000) << 13) | ((halfFloat & 0x03FF) << 13);
				return *reinterpret_cast<float32*>(&tt);
			}
			
			//uint16 Sign : 1, Exponent : 5, Fraction : 10;
			uint16 halfFloat;
		private:
			//float16(const uint32 x) : Sign((x >> 16) & 0x8000), Exponent((((x & 0x7f800000) - 0x38000000) >> 13) & 0x7c00), Fraction((x >> 13) & 0x03ff)
			//{}

			float16(const uint32 x) : halfFloat(((x >> 16) & 0x8000) | ((((x & 0x7f800000) - 0x38000000) >> 13) & 0x7c00) | ((x >> 13) & 0x03ff))
			{}
		};
		
		inline float32 Power(float32 a, const uint32 times) {
			if (!times) { return 1.0f; }
			for (uint32 i = 0; i < times - 1; i++) a *= a;
			return a;
		}

		inline constexpr float64 PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286;
		inline constexpr float64 e = 2.718281828459045235360287471352662497757247093699959574966967627724076630353;
		//inline constexpr float32 Epsilon = 2.718281828459045235360287471352662497757247093699959574966967627724076630353;

		inline constexpr Vector3 Right = Vector3(1.0f, 0.0f, 0.0f);
		inline constexpr Vector3 Up = Vector3(0.0f, 1.0f, 0.0f);
		inline constexpr Vector3 Forward = Vector3(0.0f, 0.0f, 1.0f);
		
		inline int64 Random()
		{
			static thread_local int64 x = 123456789, y = -362436069, z = 521288629;

			//period 2^96-1
			
			int64 t{ 0 };
			x ^= x << 16; x ^= x >> 5; x ^= x << 1;

			t = x; x = y; y = z; z = t ^ x ^ y;

			return z;
		}

		inline int64 Random(const int64 min, const int64 max) { return Random() % (max - min + 1) + min; }

		inline float64 RandomFloat() { return Random() * 0.8123495678; }

		//inline

		inline uint32 Factorial(const int8 A) {
			uint8 result = 1;
			for (uint8 i = 1; i < A + 1; ++i) result *= i + 1;
			return result;
		}

#if (!_WIN64)
		inline void RoundDown(const uint64 x, const uint64 multiple, uint64& quotient, uint64& remainder) { const uint64 rem = x % multiple; remainder = rem; quotient = x - rem; }
#endif
#if (_WIN64)
		inline void RoundDown(const uint64 x, const uint32 multiple, uint32& quotient, uint32& remainder) { quotient = _udiv64(x, multiple, &remainder); }
#endif

		inline bool IsPowerOfTwo(const uint64 n) { return (n & (n - 1)) == 0; }
		
		inline uint64 RoundUpByPowerOf2(const uint64 n, const uint64 powerOfTwo) {
			GTSL_ASSERT(IsPowerOfTwo(powerOfTwo), "Is not multiple of two!")
			return n + (powerOfTwo - 1) & ~(powerOfTwo - 1);
		}

		inline uint32 RoundUpByPowerOf2(const uint32 n, const uint32 powerOfTwo) {
			GTSL_ASSERT(IsPowerOfTwo(powerOfTwo), "Is not multiple of two!")
			return n + (powerOfTwo - 1) & ~(powerOfTwo - 1);
		}

		inline uint16 RoundUpByPowerOf2(const uint16 n, const uint16 powerOfTwo) {
			GTSL_ASSERT(IsPowerOfTwo(powerOfTwo), "Is not multiple of two!")
			return n + (powerOfTwo - 1) & ~(powerOfTwo - 1);
		}
		
		inline uint64 RoundUp(const uint64 number, const uint32 multiple) {
			const uint64 m_m_1 = multiple - 1, sum = number + m_m_1; return sum - sum % multiple;
		}
		/**
		 * \brief Returns x to the y.
		 * \param x number
		 * \param y exponent
		 * \return x ^ y
		 */
		float32 Power(float32 x, float32 y);

		/**
		 * \brief Returns the base 10 logarithm of x.
		 * \param x number
		 * \return Base 10 logarithm of x
		 */
		float32 Log10(float32 x);

		template<typename T>
		constexpr T Square(const T a) { return a * a; }
		
		inline bool IsEven(int32 a) { return a & 1; }

		inline float32 Floor(const float32 a) {
#ifdef _M_CEE
			return _CSTD floorf(_Xx);
#elif defined(__clang__)
			return __builtin_floorf(_Xx);
#else // ^^^ __clang__ / !__clang__ vvv
			return __floorf(a);
			//return a > 0 ? static_cast<float32>(static_cast<int32>(a)) : static_cast<float32>(static_cast<int32>(a) - 1);
#endif // __clang__
		}
		
		inline float32 Modulo(const float32 a, const float32 b) {
			const float32 c = a / b;
			return (c - Floor(c)) * b;
		}

		inline float32 Wrap(const float32 a, const float32 range) {
			return Modulo(a - range, range * 2) - range;
		}

		inline Vector2 Wrap(const Vector2 a, const Vector2 range) {
			return Vector2(Wrap(a.X(), range.X()), Wrap(a.Y(), range.Y()));
		}
		
		/**
		 * \brief Returns the sine of an angle.
		 * \param radians Angle in radians.
		 * \return Sine of radians
		 */
		//inline float32 Sine(float32 x) {
		//	float32 c; int32 intC;
		//	x = AdvancedMath::Modulo(x, static_cast<float32>(PI), c, intC);
		//	constexpr float32 B = 4.0f / static_cast<float32>(PI);
		//	constexpr float32 C = -4.0f / Square(static_cast<float32>(PI));
		//	constexpr float32 P = 0.225f;
		//
		//	float32 y = B * x + C * x * x;
		//	auto res = P * (y * y - y) + y;
		//	return IsEven(intC) ? res : -res;
		//}
		
		inline uint32 Abs(const int32 a) { return static_cast<uint32>(a < 0.0f ? -a : a); }

		inline uint64 Abs(const int64 a) { return static_cast<uint64>(a < 0.0f ? -a : a); }

		inline float32 Abs(const float32 a) { return a < 0.0f ? -a : a; }
		
		float32 Sine(float32 x);
		
		//inline float32 Sine(float32 x) {
		//	constexpr float32 B = 4.0f / PI, C = -4.0f / Square(PI), P = 0.225f;
		//	x = Wrap(x, PI);
		//	float32 y = B * x + C * Abs(x) * x;
		//	return P * (y * Abs(y) - y) + y;
		//}

		//http://lolengine.net/blog/2011/12/21/better-function-approximations
		inline float32 sin(float32 x) {
			x = Wrap(x, static_cast<float32>(PI));

			const float32 x2 = x * x, x4 = x2 * x2, x8 = x4 * x4, x9 = x8 * x;
			const auto a = x * (1.0f + x2 * (-1.666666666640169148537065260055e-1f + x2 * (8.333333316490113523036717102793e-3f + x2 * -1.984126600659171392655484413285e-4f)));
			const auto b = 2.755690114917374804474016589137e-6f + x2 * (-2.502845227292692953118686710787e-8f + x2 * 1.538730635926417598443354215485e-10f);
			return a + x9 * b;
		}

		/**
		 * \brief Calculates the sine of an angle in a clockwise manner.
		 * \param x angle in radians
		 * \return The sine of x.
		 */
		inline float32 sin_CW(Radians radians) {
			auto x = radians();
			x = Wrap(x, static_cast<float32>(PI));

			const float32 x2 = x * x, x4 = x2 * x2, x8 = x4 * x4, x9 = x8 * x;
			const auto a = x * (-1.0f + x2 * (1.666666666640169148537065260055e-1f + x2 * (-8.333333316490113523036717102793e-3f + x2 * 1.984126600659171392655484413285e-4f)));
			const auto b = -2.755690114917374804474016589137e-6f + x2 * (2.502845227292692953118686710787e-8f + x2 * -1.538730635926417598443354215485e-10f);
			return a + x9 * b;
		}

		inline float32 cos(const float32 x) {
			return sin(x + static_cast<float32>(PI) / 2.0f);
		}

		inline float32 cos_CW(const float32 x) {
			return sin_CW(Radians(x + static_cast<float32>(PI) / 2.0f));
		}
		
		/**
		 * \brief Returns the sine of an angle.
		 * \param radians Angle in radians.
		 * \return Sine of radians
		 */
		float64 Sine(float64 radians);

		/**
		* \brief Returns the cosine of an angle.
		* \param radians Angle in radians.
		* \return Cosine of radians
		*/
		float32 Cosine(float32 x);
		//inline float32 Cosine(float32 x) {
		//	x = (x > 0) ? -x : x;
		//	return Sine(x + static_cast<float32>(PI) * 0.5f);
		//}

		/**
		* \brief Returns the cosine of an angle.
		* \param radians Angle in radians.
		* \return Cosine of radians
		*/
		float64 Cosine(float64 radians);

		/**
		* \brief Returns the tangent of an angle.
		* \param radians Angle in radians.
		* \return Tangent of radians
		*/
		//inline float32 Tangent(float32 x) {
			//return Sine(x) / Cosine(x);
		//}

		float32 Tangent(float32 x);
		
		/**
		* \brief Returns the tangent of an angle.
		* \param radians Angle in radians.
		* \return Tangent of radians
		*/
		float64 Tangent(float64 radians);

		/**
		* \brief Returns the arcsine of A in radians.
		* \param A value
		* \return Radians of A
		*/
		float32 ArcSine(float32 A);

		/**
		* \brief Returns the arccosine of A in radians.
		* \param A value
		* \return Radians of A
		*/
		float32 ArcCosine(float32 A);

		/**
		* \brief Returns the arctangent of A in radians.
		* \param A value
		* \return Radians of A
		*/
		float32 ArcTangent(float32 A);

		/**
		* \brief Returns the arctangent of Y / X in degrees.
		* \return Degrees of Y / X
		*/
		float32 ArcTan2(float32 y, float32 x);

		inline void Sine(Range<float32*> n) {
			using SIMD = SIMD128<float32>;

			uint32 i = 0;

			const SIMD a0(1.0f), a1(-1.666666666640169148537065260055e-1f), a2(8.333333316490113523036717102793e-3f),
				a3(-1.984126600659171392655484413285e-4f),
				a4(2.755690114917374804474016589137e-6f),
				a5(-2.502845227292692953118686710787e-8f),
				a6(1.538730635926417598443354215485e-10f);

			for (uint32 t = 0; t < n.ElementCount() / SIMD::TypeElementsCount; ++t, i += SIMD::TypeElementsCount) {
				auto x = SIMD(AlignedPointer<const float32, 16>(n.begin() + i));

				//wrap modulo PI
				auto c = (x - SIMD(static_cast<float32>(PI))) / SIMD(static_cast<float32>(PI) * 2.0f);
				x = (c - SIMD::Floor(c)) * SIMD(static_cast<float32>(PI) * 2.0f);
				//wrap modulo PI

				const SIMD x2 = x * x, x4 = x2 * x2, x8 = x4 * x4, x9 = x8 * x;
				const auto a = x * (a0 + x2 * (a1 + x2 * (a2 + x2 * a3)));
				const auto b = a4 + x2 * (a5 + x2 * a6);
				(a + x9 * b).CopyTo(AlignedPointer<float32, 16>(n.begin() + i));
			}

			for (; i < n.ElementCount(); ++i) {
				n[i] = Sine(n[i]);
			}
		}
		
		inline float32 SquareRoot(const float32 a) {
			if (a > 0.0f) { return AdvancedMath::SquareRoot(a); }
			return 0.0f;
		}

		inline float64 SquareRoot(const float64 a) {
			if (a > 0.0) { return AdvancedMath::SquareRoot(a); }
			return 0.0;
		}
		
		/**
		 * \brief Computes a square root via IEEE754 bit hacking which is faster but less accurate.
		 * \param x Number to compute the square root of
		 * \return The square root of x
		 */
		inline float32 FastSqrt(const float32 x) {
			if (x > 0.0f) { return AdvancedMath::FastSqrt(x); }
			return 0.0f;
		}
		
		inline Vector2 Modulo(const Vector2 a, const Vector2 b) {
			return Vector2(Modulo(a.X(), b.X()), Modulo(a.Y(), b.Y()));
		}
		
		//////////////////////////////////////////////////////////////
		//						SCALAR MATH							//
		//////////////////////////////////////////////////////////////

		template<typename T>
		void MinMax(T a, T b, T& min, T& max) {
			if (a < b) { min = a; max = b; }
			else { max = a; min = b; }
		}

		template<>
		inline void MinMax(Vector2 a, Vector2 b, Vector2& min, Vector2& max) {
			MinMax(a.X(), b.X(), min.X(), max.X()); MinMax(a.Y(), b.Y(), min.Y(), max.Y());
		}

		//Returns 1 if A is bigger than 0. 0 if A is equal to 0. and -1 if A is less than 0.
		inline int8 Sign(const int64 A) {
			if (A > 0) { return 1; } if (A < 0) { return -1; } return 0;
		}

		//Returns 1 if A is bigger than 0. 0 if A is equal to 0. and -1 if A is less than 0.
		inline int8 Sign(const float32 A)
		{
			if (A > 0.0f)			
				return 1;
			
			if (A < 0.0f)
				return -1;

			return 0;
		}
		
		//Mixes A and B by the specified values, Where Alpha 0 returns A and Alpha 1 returns B.
		inline float32 Lerp(const float32 a, const float32 b, const float32 alpha) {
			return a + alpha * (b - a);
		}

		inline Vector2 Lerp(const Vector2 a, const Vector2 b, const float32 alpha) {
			return Vector2(Lerp(a.X(), b.X(), alpha), Lerp(a.Y(), b.Y(), alpha));
		}

		inline Vector3 Lerp(const Vector3 a, const Vector3 b, const float32 alpha) {
			return Vector3(Lerp(a.X(), b.X(), alpha), Lerp(a.Y(), b.Y(), alpha), Lerp(a.Z(), b.Z(), alpha));
		}

		inline Vector4 Lerp(const Vector4 a, const Vector4 b, const float32 alpha) {
			return Vector4(Lerp(a.X(), b.X(), alpha), Lerp(a.Y(), b.Y(), alpha), Lerp(a.Z(), b.Z(), alpha), Lerp(a.W(), b.W(), alpha));
		}

		inline float32 LengthSquared(const Vector2 a) { return a.X() * a.X() + a.Y() * a.Y(); }
		inline float32 LengthSquared(const Vector2 a, const Vector2 b) { return LengthSquared(b - a); }
		inline float32 LengthSquared(const Vector3 a) { return a.X() * a.X() + a.Y() * a.Y() + a.Z() * a.Z(); }
		inline float32 LengthSquared(const Vector3 a, const Vector3 b) { return LengthSquared(b - a); }
		inline float32 LengthSquared(const Vector4 a) { return a.X() * a.X() + a.Y() * a.Y() + a.Z() * a.Z() + a.W() * a.W(); }
		inline float32 LengthSquared(const Vector4 a, const Vector4 b) { return LengthSquared(b - a); }

		inline float32 Length(const Vector2 a) { return SquareRoot(LengthSquared(a)); }
		inline float32 Length(const Vector2 a, const Vector2 b) { return SquareRoot(LengthSquared(a, b)); }
		inline float32 Length(const Vector3 a) { return SquareRoot(LengthSquared(a)); }
		inline float32 Length(const Vector3 a, const Vector3 b) { return SquareRoot(LengthSquared(a, b)); }
		inline float32 Length(const Vector4 a) { return SquareRoot(LengthSquared(a)); }
		inline float32 Length(const Vector4 a, const Vector4 b) { return SquareRoot(LengthSquared(a, b)); }

		inline Vector2 Normalized(const Vector2& a) {
			auto length = Length(a); if (length == 0.0f) { return a; }
			length = 1.0f / length;
			return Vector2(a.X() * length, a.Y() * length);
		}

		inline void Normalize(Vector2& a) {
			auto length = Length(a);
			if (length == 0.0f) { return; }
			length = 1.0f / length;
			a.X() *= length; a.Y() *= length;
		}

		inline Vector3 Normalized(const Vector3& a) {
			auto length = Length(a); if (length == 0.0f) { return a; }
			length = 1.0f / length;
			return Vector3(a.X() * length, a.Y() * length, a.Z() * length);
		}

		inline void Normalize(Vector3& a) {
			auto length = Length(a);
			if (length == 0.0f) { return; }
			length = 1.0f / length;
			a.X() *= length; a.Y() *= length; a.Z() *= length;
		}

		inline Vector4 Normalized(const Vector4& a) {
			auto length = Length(a); if (length == 0.0f) { return a; }
			length = 1.0f / length;
			return Vector4(a.X() * length, a.Y() * length, a.Z() * length, a.W() * length);
		}

		inline void Normalize(Vector4& a) {
			auto length = Length(a);
			if (length == 0.0f) { return; }
			length = 1.0f / length;
			a.X() *= length; a.Y() *= length; a.Z() *= length; a.W() *= length;
		}

		inline void Normalize(Quaternion& a) {
			auto length = Length(a);
			if (length == 0.0f) { a.X() = 1.0f; return; }
			length = 1.0f / length;
			a.X() *= length; a.Y() *= length; a.Z() *= length; a.W() *= length;
		}
		
		inline Quaternion Normalized(const Quaternion& a) {
			auto length = Length(a); if (length == 0.0f) { return a; }
			length = 1.0f / length;
			return Quaternion(a.X() * length, a.Y() * length, a.Z() * length, a.W() * length);
		}
		
		inline Quaternion Lerp(const Quaternion a, const Quaternion b, const float32 alpha) {
			return Normalized(Quaternion(Lerp(a.X(), b.X(), alpha), Lerp(a.Y(), b.Y(), alpha), Lerp(a.Z(), b.Z(), alpha), Lerp(a.W(), b.W(), alpha)));
		}

		inline float32 Square(const float32 a) { return a * a; }

		//Interpolates from Current to Target, returns Current + an amount determined by the InterpSpeed.
		inline float32 Interp(const float32 Target, const float32 Current, const float32 DT, const float32 InterpSpeed) {
			return AdvancedMath::Interp(Target, Current, DT * InterpSpeed);
		}

		inline Vector2 Interp(const Vector2 target, const Vector2 current, const float32 deltaTime, const float32 speed) {
			auto factor = deltaTime * speed;
			return Vector2(AdvancedMath::Interp(target.X(), current.X(), factor), AdvancedMath::Interp(target.Y(), current.Y(), factor));
		}

		inline Vector3 Interp(const Vector3 target, const Vector3 current, const float32 deltaTime, const float32 speed) {
			auto factor = deltaTime * speed;
			return Vector3(AdvancedMath::Interp(target.X(), current.X(), factor), AdvancedMath::Interp(target.Y(), current.Y(), factor),
				AdvancedMath::Interp(target.Z(), current.Z(), factor));
		}

		inline Vector4 Interp(const Vector4 target, const Vector4 current, const float32 deltaTime, const float32 speed) {
			auto factor = deltaTime * speed;
			return Vector4(AdvancedMath::Interp(target.X(), current.X(), factor), AdvancedMath::Interp(target.Y(), current.Y(), factor),
				AdvancedMath::Interp(target.Z(), current.Z(), factor), AdvancedMath::Interp(target.W(), current.W(), factor));
		}

		inline float32 InterpDelta(const float32 target, const float32 current, const float32 deltaTime, const float32 speed) {
			return AdvancedMath::InterpDelta(target, current, deltaTime * speed);
		}

		inline Vector2 InterpDelta(const Vector2 target, const Vector2 current, const float32 deltaTime, const float32 speed) {
			auto factor = deltaTime * speed;
			return Vector2(AdvancedMath::InterpDelta(target.X(), current.X(), factor), AdvancedMath::InterpDelta(target.Y(), current.Y(), factor));
		}

		inline Vector3 InterpDelta(const Vector3 target, const Vector3 current, const float32 deltaTime, const float32 speed) {
			auto factor = deltaTime * speed;
			return Vector3(AdvancedMath::InterpDelta(target.X(), current.X(), factor), AdvancedMath::InterpDelta(target.Y(), current.Y(), factor),
				AdvancedMath::InterpDelta(target.Z(), current.Z(), factor));
		}

		inline Vector4 InterpDelta(const Vector4 target, const Vector4 current, const float32 deltaTime, const float32 speed) {
			auto factor = deltaTime * speed;
			return Vector4(AdvancedMath::InterpDelta(target.X(), current.X(), factor), AdvancedMath::InterpDelta(target.Y(), current.Y(), factor),
				AdvancedMath::InterpDelta(target.Z(), current.Z(), factor), AdvancedMath::InterpDelta(target.W(), current.W(), factor));
		}
		
		inline uint32 InvertRange(const uint32 a, const uint32 max) { return max - a; }

		inline float32 InvertRange(const float32 a, const float32 max) { return max - a; }
		
		inline float32 MapToRange(const float32 x, const float32 inMin, const float32 inMax, const float32 outMin, const float32 outMax)
		{
			if ((inMax - inMin) != 0.0f) {
				GTSL_ASSERT(x >= inMin && x <= inMax, "Not in range");
				return (x - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
			}
			return x;
		}

		inline Vector2 MapToRange(const Vector2 a, const Vector2 matrixin, const Vector2 matrixax, const Vector2 OutMin, const Vector2 OutMax) {
			return Vector2(MapToRange(a.X(), matrixin.X(), matrixax.X(), OutMin.X(), OutMax.X()), MapToRange(a.Y(), matrixin.Y(), matrixax.Y(), OutMin.Y(), OutMax.Y()));
		}

		inline float32 MapToRangeZeroToOne(const float32 a, const float32 inMax, const float32 outMax) { return a / (inMax / outMax); }

		inline float32 Root(const float32 a, const float32 root) { return Power(a, 1.0f / root); }

		template<typename T>
		T Limit(const T a, const T max) { return a > max ? max : a; }
		
		template <typename T>
		T Min(const T& A, const T& B) { return (A < B) ? A : B; }

		template <typename T>
		T Max(const T& A, const T& B) { return (A > B) ? A : B; }

		inline Vector2 Min(const Vector2 a, const Vector2 b) { return Vector2(Min(a.X(), b.X()), Min(a.Y(), b.Y())); }
		inline Vector2 Max(const Vector2 a, const Vector2 b) { return Vector2(Max(a.X(), b.X()), Max(a.Y(), b.Y())); }
		
		inline Vector3 Min(const Vector3 a, const Vector3 b) { return Vector3(Min(a.X(), b.X()), Min(a.Y(), b.Y()), Min(a.Z(), b.Z())); }
		inline Vector3 Max(const Vector3 a, const Vector3 b) { return Vector3(Max(a.X(), b.X()), Max(a.Y(), b.Y()), Max(a.Z(), b.Z())); }

		inline Vector4 Min(const Vector4 a, const Vector4 b) { return Vector4(Min(a.X(), b.X()), Min(a.Y(), b.Y()), Min(a.Z(), b.Z()), Min(a.W(), b.W())); }
		inline Vector4 Max(const Vector4 a, const Vector4 b) { return Vector4(Max(a.X(), b.X()), Max(a.Y(), b.Y()), Max(a.Z(), b.Z()), Max(a.W(), b.W())); }
		
		/**
		 * \brief Returns degrees converted to radians.
		 * \param degrees degrees to convert
		 * \return Degrees as radians
		 */
		inline float32 DegreesToRadians(const float32 degrees) { return degrees * static_cast<float32>(PI / 180.0); }

		/**
		 * \brief Returns degrees converted to radians.
		 * \param degrees degrees to convert
		 * \return Degrees as radians
		 */
		inline float64 DegreesToRadians(const float64 degrees) { return degrees * (PI / 180.0); }

		/**
		 * \brief Returns Radians converted to degrees.
		 * \param radians radians to convert.
		 * \return Radians as degrees.
		 */
		inline float32 RadiansToDegrees(const float32 radians) { return radians * static_cast<float32>(180.0 / PI); }

		/**
		 * \brief Returns Radians converted to degrees.
		 * \param radians radians to convert.
		 * \return Radians as degrees.
		 */
		inline float64 RadiansToDegrees(const float64 radians) { return radians * (180.0 / PI); }

		//////////////////////////////////////////////////////////////
		//						VECTOR MATH							//
		//////////////////////////////////////////////////////////////

		inline float32 DotProduct(const Vector2 a, const Vector2 b) { return a.X() * b.X() + a.Y() * b.Y(); }
		inline float32 DotProduct(const Vector3& a, const Vector3& b) { return a.X() * b.X() + a.Y() * b.Y() + a.Z() * b.Z(); }
		inline float32 DotProduct(const Vector4& a, const Vector4& b) { return a.X() * b.X() + a.Y() * b.Y() + a.Z() * b.Z() + a.W() * b.W(); }

		inline Vector3 Cross(const Vector3& a, const Vector3& b) { return Vector3(a.Y() * b.Z() - a.Z() * b.Y(), a.Z() * b.X() - a.X() * b.Z(), a.X() * b.Y() - a.Y() * b.X()); }
		inline Vector3 TripleProduct(const Vector3& a, const Vector3& b) { return Cross(Cross(a, b), b); }

		inline Vector2 Abs(const Vector2 a) { return Vector2(Abs(a.X()), Abs(a.Y())); }
		inline Vector3 Abs(const Vector3 a) { return Vector3(Abs(a.X()), Abs(a.Y()), Abs(a.Z())); }
		inline Vector4 Abs(const Vector4 a) { return Vector4(Abs(a.X()), Abs(a.Y()), Abs(a.Z()), Abs(a.W())); }

		inline void Negate(float32& a) { a *= -1.0f; }

		inline Vector2 Negated(const Vector2& a) {
			return Vector2(-a.X(), -a.Y());
		}

		inline void Negate(Vector2& a) { a.X() = -a.X(); a.Y() = -a.Y(); }

		inline Vector3 Negated(const Vector3& a) {
			return Vector3(-a.X(), -a.Y(), -a.Z());
		}

		inline void Negate(Vector3& a) { a.X() = -a.X(); a.Y() = -a.Y(); a.Z() = -a.Z(); }

		inline Vector4 Negated(const Vector4& a) {
			return Vector4(-a.X(), -a.Y(), -a.Z(), -a.W());
		}

		inline void Negate(Vector4& a) {
			a.X() = -a.X(); a.Y() = -a.Y(); a.Z() = -a.Z(); a.W() = -a.W();
		}

		//////////////////////////////////////////////////////////////
		//						QUATERNION MATH						//
		//////////////////////////////////////////////////////////////

		inline Quaternion Conjugated(const Quaternion& a) { return Quaternion(-a.X(), -a.Y(), -a.Z(), a.W()); }

		inline void Conjugate(Quaternion& a) { a.X() = -a.X(); a.Y() = -a.Y(); a.Z() = -a.Z(); }

		inline Quaternion Slerp(Quaternion a, Quaternion b, float32 alpha) {
			//Implementation from assimp
			// Calculate angle between them.
			float32 cosom = DotProduct(a, b);
			if (cosom < 0.0f) {
				Negate(b);
				Negate(cosom);
			}

			float32 sclp, sclq;

			// Calculate coefficients
			if ((static_cast<float32>(1.0) - cosom) > static_cast<float32>(0.0001)) { // 0.0001 -> some epsillon
				// Standard case (slerp)
				float32 omega, sinom;
				omega = ArcCosine(cosom); // extract theta from dot product's cos theta
				sinom = Sine(omega);
				sclp = Sine((static_cast<float32>(1.0) - alpha) * omega) / sinom;
				sclq = Sine(alpha * omega) / sinom;
			} else {
				// Very close, do linear interp (because it's faster)
				sclp = static_cast<float32>(1.0) - alpha;
				sclq = alpha;
			}

			Quaternion result;
			result.X() = sclp * a.X() + sclq * b.X();
			result.Y() = sclp * a.Y() + sclq * b.Y();
			result.Z() = sclp * a.Z() + sclq * b.Z();
			result.W() = sclp * a.W() + sclq * b.W();

			return result;
		}

		//////////////////////////////////////////////////////////////
		//						LOGIC								//
		//////////////////////////////////////////////////////////////

		//Compares float numbers with adaptive epsilon based on the scale of the numbers
		inline bool Compare(float32 f1, float32 f2)
		{
			static constexpr auto epsilon = 1.0e-05f;
			if (Abs(f1 - f2) <= epsilon)
				return true;
			return Abs(f1 - f2) <= epsilon * Max(Abs(f1), Abs(f2));
		}
		
		inline bool IsNearlyEqual(const float32 A, const float32 Target, const float32 Tolerance) {
			return (A >= Target - Tolerance) && (A <= Target + Tolerance);
		}

		inline bool IsInRange(const float32 A, const float32 Min, const float32 Max) {
			return (A >= Min) && (A <= Max);
		}

		inline bool IsNearlyEqual(const Vector2& A, const Vector2& Target, const float32 Tolerance) {
			return IsNearlyEqual(A.X(), Target.X(), Tolerance) && IsNearlyEqual(A.Y(), Target.Y(), Tolerance);
		}

		inline bool IsNearlyEqual(const Vector3& A, const Vector3& Target, const float32 Tolerance) {
			if (IsNearlyEqual(A.X(), Target.X(), Tolerance))
				if (IsNearlyEqual(A.Y(), Target.Y(), Tolerance))
					if (IsNearlyEqual(A.Z(), Target.Z(), Tolerance))
						return true;
			
			return false;
		}

		inline bool IsNearlyEqual(const Vector4& a, const Vector4& b, const float32 tolerance)
		{
			if (IsNearlyEqual(a.X(), b.X(), tolerance))
				if (IsNearlyEqual(a.Y(), b.Y(), tolerance))
					if (IsNearlyEqual(a.Z(), b.Z(), tolerance))
						if (IsNearlyEqual(a.W(), b.W(), tolerance))
							return true;

			return false;
		}

		inline bool AreVectorComponentsGreater(const Vector3& A, const Vector3& B) {
			return A.X() > B.X() && A.Y() > B.Y() && A.Z() > B.Z();
		}

		inline bool PointInBox(Vector2 min, Vector2 max, Vector2 p) { return p.X() >= min.X() && p.X() <= max.X() && p.Y() >= min.Y() && p.Y() <= max.Y(); }

		inline bool PointInBoxProjection(Vector2 min, Vector2 max, Vector2 p) { return p.X() >= min.X() && p.X() <= max.X() || p.Y() >= min.Y() && p.Y() <= max.Y(); }

		uint8 constexpr YAW = 0, PITCH = 1;

		inline Vector2 ToSphericalCoordinates(const Vector3 vector3) {
			Vector2 result;			
			result[YAW] = ArcTan2(vector3[2], vector3[0]);
			result[PITCH] = ArcTan2(vector3[2], vector3[1]);
			return result;
		}

		//////////////////////////////////////////////////////////////
		//						MATRIX MATH							//
		//////////////////////////////////////////////////////////////
		
		//Modifies the given matrix to make it a translation matrix.
		inline void AddTranslation(Matrix4& matrix, const Vector3 vector) {
			matrix(0, 3) += vector[0]; matrix(1, 3) += vector[1]; matrix(2, 3) += vector[2];
		}

		//Modifies the given matrix to make it a translation matrix.
		inline void SetTranslation(Matrix4& matrix, const Vector3 vector) {
			matrix(0, 3) = vector[0]; matrix(1, 3) = vector[1]; matrix(2, 3) = vector[2];
		}

		inline void SetTranslation(Matrix3x4& matrix, const Vector3 vector) {
			matrix[0][3] = vector[0]; matrix[1][3] = vector[1]; matrix[2][3] = vector[2];
		}

		inline void AddRotation(Matrix4& matrix, const Quaternion quaternion) {
			matrix *= Matrix4(quaternion);
		}

		inline void SetRotation(Matrix4& matrix, const Quaternion quaternion) {
			const auto xx = quaternion.X() * quaternion.X();
			const auto xy = quaternion.X() * quaternion.Y();
			const auto xz = quaternion.X() * quaternion.Z();
			const auto xw = quaternion.X() * quaternion.W();
			const auto yy = quaternion.Y() * quaternion.Y();
			const auto yz = quaternion.Y() * quaternion.Z();
			const auto yw = quaternion.Y() * quaternion.W();
			const auto zz = quaternion.Z() * quaternion.Z();
			const auto zw = quaternion.Z() * quaternion.W();

			matrix[0][0] = 1 - 2 * (yy + zz);
			matrix[0][1] = 2 * (xy - zw);
			matrix[0][2] = 2 * (xz + yw);
			matrix[1][0] = 2 * (xy + zw);
			matrix[1][1] = 1 - 2 * (xx + zz);
			matrix[1][2] = 2 * (yz - xw);
			matrix[2][0] = 2 * (xz - yw);
			matrix[2][1] = 2 * (yz + xw);
			matrix[2][2] = 1 - 2 * (xx + yy);
		}
		
		inline void AddRotation(Matrix3x4& matrix, const Quaternion quaternion) {
			matrix *= Matrix3x4(quaternion);
		}

		inline void SetRotation(Matrix3x4& matrix, const Quaternion quaternion) {
			matrix *= Matrix3x4(quaternion);
		}

		inline void AddRotation(Matrix4& matrix, const AxisAngle axisAngle) {
			matrix *= Matrix4(axisAngle);
		}

		inline void SetRotation(Matrix4& matrix, const AxisAngle axisAngle) {
			matrix *= Matrix4(axisAngle);
		}

		inline void AddRotation(Matrix4& matrix, const Rotator rotator) {
			matrix *= Matrix4(rotator);
		}

		inline void SetRotation(Matrix4& matrix, const Rotator rotator) {
			matrix *= Matrix4(rotator);
		}
		
		//Modifies the given matrix to make it a translation matrix.
		inline void AddScale(Matrix4& matrix, const Vector3 vector) {
			matrix(0, 0) *= vector[0]; matrix(0, 1) *= vector[0]; matrix(0, 2) *= vector[0]; matrix(0, 3) *= vector[0];
			matrix(1, 0) *= vector[1]; matrix(1, 1) *= vector[1]; matrix(1, 2) *= vector[1]; matrix(1, 3) *= vector[1];
			matrix(2, 0) *= vector[2]; matrix(2, 1) *= vector[2]; matrix(2, 2) *= vector[2]; matrix(2, 3) *= vector[2];
		}

		//Modifies the given matrix to make it a translation matrix.
		inline void SetScale(Matrix4& matrix, const Vector3 vector) {
			matrix(0, 0) = vector[0]; matrix(1, 1) = vector[1]; matrix(2, 2) = vector[2];
		}
		
		inline Matrix4 NormalToRotation(Vector3 normal) {
			// Find a vector in the plane
			Vector3 tangent0 = Cross(normal, Vector3(1, 0, 0));
			if (DotProduct(tangent0, tangent0) < 0.001)
				tangent0 = Cross(normal, Vector3(0, 1, 0));
			Normalize(tangent0);
			// Find another vector in the plane
			const Vector3 tangent1 = Normalized(Cross(normal, tangent0));
			return Matrix4(tangent0.X(), tangent0.Y(), tangent0.Z(), 0.0f, tangent1.X(), tangent1.Y(), tangent1.Z(), 0.0f, normal.X(), normal.Y(), normal.Z(), 0.0f, 0, 0, 0, 0);
		}

		inline void Rotate(Matrix4& matrix, const Quaternion& quaternion) {
			matrix *= Matrix4(quaternion);
		}

		inline Vector3 GetTranslation(const Matrix4& matrix4) { return Vector3(matrix4[0][3], matrix4[1][3], matrix4[2][3]); }
		
		//Returns point colinearity to a line defined by two points.
		// +0 indicates point is to the right
		// 0 indicates point is on the line
		// -0 indicates point is to the left
		inline float32 TestPointToLineSide(const Vector2 a, const Vector2 b, const Vector2 p) {
			return ((a.X() - b.X()) * (p.Y() - b.Y()) - (a.Y() - b.Y()) * (p.X() - b.X()));
		};

		inline Vector3 SphericalCoordinatesToCartesianCoordinates(const Vector2& sphericalCoordinates) {
			const auto cy = Cosine(sphericalCoordinates.Y());
			return Vector3(cy * Sine(sphericalCoordinates.X()), Sine(sphericalCoordinates.Y()),	cy * Cosine(sphericalCoordinates.X()));
		}

		inline Matrix4 Scaling(const Vector3& A) {
			Matrix4 Result(1.0f);
			Result[0][0] = A.X(); Result[1][1] = A.Y(); Result[2][2] = A.Z();
			return Result;
		}
		
		inline void Scale(Matrix4& matrix, const Vector3& scale) {
			matrix *= Scaling(scale);
		}

		/**
		 * \brief Builds a left handed, 0 to 1 perspective projection matrix based on some camera and screen parameters.
		 * \param fov Camera vertical field of view.
		 * \param aspectRatio Screen aspect ratio.
		 * \param nearPlane Distance between the near clipping plane and the origin of the camera.
		 * \param farPlane Distance between the far clipping plane and the origin of the camera.
		 * \return Perspective projection matrix.
		 */
		inline Matrix4 BuildPerspectiveMatrix(const float32 fov, const float32 aspectRatio, const float32 nearPlane, const float32 farPlane) {
			Matrix4 matrix;
			
			//Tangent of half the vertical view angle.
			const auto yScale = 1.0f / Tangent(fov * 0.5f);
			const auto xScale = yScale / aspectRatio;

			matrix[0][0] = xScale; matrix[1][1] = yScale;
			matrix[2][2] = farPlane / (farPlane - nearPlane);
			matrix[2][3] = (nearPlane * farPlane) / (nearPlane - farPlane);
			matrix[3][2] = 1.0f; matrix[3][3] = 0.0f;
			
			return matrix;
		}
		
		inline Matrix4 BuildInversePerspectiveMatrix(const float32 fov, const float32 aspectRatio, const float32 nearPlane, const float32 farPlane) {
			Matrix4 matrix;

			const auto yScale = 1.0f / Tangent(fov * 0.5f);
			const auto xScale = yScale / aspectRatio;

			matrix[0][0] = 1 / xScale; matrix[1][1] = 1 / yScale;
			auto z0 = farPlane / (farPlane - nearPlane);
			auto z1 = (nearPlane * farPlane) / (nearPlane - farPlane);

			matrix[3][2] = 1 / z1;
			matrix[2][3] = 1.0f;
			
			matrix[3][3] = z0 / z1;
			
			return matrix;
		}

		inline Matrix4 MakeOrthoMatrix(const float32 right, const float32 left, const float32 top, const float32 bottom, const float32 nearPlane, const float32 farPlane) {
			Matrix4 matrix;
			matrix[0][0] = static_cast<float32>(2) / (right - left);
			matrix[1][1] = static_cast<float32>(2) / (top - bottom);
			matrix[2][2] = static_cast<float32>(1) / (farPlane - nearPlane);
			matrix[3][0] = -(right + left) / (right - left);
			matrix[3][1] = -(top + bottom) / (top - bottom);
			matrix[3][2] = -nearPlane / (farPlane - nearPlane);

			return matrix;
		}

		inline Matrix4 MakeOrthoMatrix(const Extent2D extent, const float32 nearPlane, const float32 farPlane) {
			Vector2 xy(extent.Width, extent.Height); xy /= 2.0f;
			return MakeOrthoMatrix(xy.X(), -xy.X(), xy.Y(), -xy.X(), nearPlane, farPlane);
		}
		
		template<typename T>
		T Clamp(T a, T min, T max) { return a > max ? max : (a < min ? min : a); }

		inline Vector3 ClosestPointOnPlane(const Vector3& point, const Plane& plane)
		{
			const float32 T = (DotProduct(plane.Normal, point) - plane.D) / DotProduct(plane.Normal, plane.Normal);
			return point - plane.Normal * T;
		}

		inline float32 DistanceFromPointToPlane(const Vector3& point, const Plane& plane)
		{
			// return Dot(q, p.n) - p.d; if plane equation normalized (||p.n||==1)
			return (DotProduct(plane.Normal, point) - plane.D) / DotProduct(plane.Normal, plane.Normal);
		}

		inline Vector2 ClosestPointOnLineToPoint(const Vector2 a, const Vector2 b, const Vector2 p)
		{
			const auto m = b - a;
			const auto t0 = DotProduct(m, p - a) / DotProduct(m, m);
			return a + m * t0;
		}

		inline Vector2 ClosestPointOnLineSegmentToPoint(const Vector2 a, const Vector2 b, const Vector2 p)
		{
			const auto AB = b - a;
			// Project c onto ab, computing parameterized position d(t) = a + t*(b – a)
			auto t = DotProduct(p - a, AB) / LengthSquared(AB);
			// If outside segment, clamp t (and therefore d) to the closest endpoint
			if (t < 0.0f) { return a; }
			if (t > 1.0f) { return b; }
			// Compute projected position from the clamped t
			return a + AB * t;
		}
		
		inline Vector2 ClosestPointOnLineSegmentToPoint(const Vector2 a, const Vector2 b, const Vector2 p, float32& isOnLine)
		{
			const auto AB = b - a;
			// Project c onto ab, computing parameterized position d(t) = a + t*(b – a)
			auto t = DotProduct(p - a, AB) / LengthSquared(AB);
			// If outside segment, clamp t (and therefore d) to the closest endpoint
			if (t < 0.0f) { isOnLine = 0.0f; return a; }
			if (t > 1.0f) { isOnLine = 0.0f; return b; }

			isOnLine = 1.0f;
			// Compute projected position from the clamped t
			return a + AB * t;
		}

		inline Vector3 ClosestPointOnLineSegmentToPoint(const Vector3& a, const Vector3& b, const Vector3& p)
		{
			const Vector3 AB = b - a;
			// Project c onto ab, computing parameterized position d(t) = a + t*(b – a)
			auto t = DotProduct(p - a, AB) / LengthSquared(AB);
			// If outside segment, clamp t (and therefore d) to the closest endpoint
			if (t < 0.0f) t = 0.0f;
			if (t > 1.0f) t = 1.0f;
			// Compute projected position from the clamped t
			return a + AB * t;
		}

		inline float32 SquaredDistancePointToSegment(const Vector3& a, const Vector3& b, const Vector3& c)
		{
			const Vector3 AB = b - a;
			const Vector3 AC = c - a;
			const Vector3 BC = c - b;
			const float32 E = DotProduct(AC, AB);
			// Handle cases where c projects outside ab
			if (E <= 0.0f) return DotProduct(AC, AC);
			const float32 f = DotProduct(AB, AB);
			if (E >= f) return DotProduct(BC, BC);
			// Handle cases where c projects onto ab
			return DotProduct(AC, AC) - E * E / f;
		}

		// Compute barycentric coordinates (u, v, w) for
		// point p with respect to triangle (a, b, c)
		inline void Barycentric(Vector2 a, Vector2 b, Vector2 c, Vector2 p, float32& s, float32& t, float32& u)
		{
			Vector2 v0 = b - a, v1 = c - a, v2 = p - a;
			float32 den = v0.X() * v1.Y() - v1.X() * v0.Y();
			s = (v2.X() * v1.Y() - v1.X() * v2.Y()) / den;	
			t = (v0.X() * v2.Y() - v2.X() * v0.Y()) / den;
			u = 1.0f - s - t;
		}

		inline Vector3 ClosestPointOnTriangleToPoint(const Vector3& a, const Vector3& p1, const Vector3& p2, const Vector3& p3)
		{
			// Check if P in vertex region outside A
			const Vector3 AP = a - p1;
			const Vector3 AB = p2 - p1;
			const Vector3 AC = p3 - p1;

			const float32 D1 = DotProduct(AB, AP);
			const float32 D2 = DotProduct(AC, AP);
			if (D1 <= 0.0f && D2 <= 0.0f) return p1; // barycentric coordinates (1,0,0)

			// Check if P in vertex region outside B
			const Vector3 BP = a - p2;
			const float32 D3 = DotProduct(AB, BP);
			const float32 D4 = DotProduct(AC, BP);
			if (D3 >= 0.0f && D4 <= D3) return p2; // barycentric coordinates (0,1,0)

			// Check if P in edge region of AB, if so return projection of P onto AB
			const float32 VC = D1 * D4 - D3 * D2;
			if (VC <= 0.0f && D1 >= 0.0f && D3 <= 0.0f) {
				const float32 V = D1 / (D1 - D3);
				return p1 + AB * V; // barycentric coordinates (1-v,v,0)
			}

			// Check if P in vertex region outside C
			const Vector3 CP = a - p3;
			const float32 D5 = DotProduct(AB, CP);
			const float32 D6 = DotProduct(AC, CP);
			if (D6 >= 0.0f && D5 <= D6) return p3; // barycentric coordinates (0,0,1)

			// Check if P in edge region of AC, if so return projection of P onto AC
			const float32 VB = D5 * D2 - D1 * D6;
			if (VB <= 0.0f && D2 >= 0.0f && D6 <= 0.0f) {
				const float32 W = D2 / (D2 - D6);
				return p1 + AC * W; // barycentric coordinates (1-w,0,w)
			}

			// Check if P in edge region of BC, if so return projection of P onto BC
			const float32 VA = D3 * D6 - D5 * D4;
			if (VA <= 0.0f && (D4 - D3) >= 0.0f && (D5 - D6) >= 0.0f) {
				const float32 W = (D4 - D3) / ((D4 - D3) + (D5 - D6));
				return p2 + (p3 - p2) * W; // barycentric coordinates (0,1-w,w)
			}

			// P inside face region. Compute Q through its barycentric coordinates (u,v,w)
			const float32 Denom = 1.0f / (VA + VB + VC);
			const float32 V = VB * Denom;
			const float32 W = VC * Denom;
			return p1 + AB * V + AC * W; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
		}

		inline bool PointOutsideOfPlane(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
		{
			return DotProduct(p - a, Cross(b - a, c - a)) >= 0.0f; // [AP AB AC] >= 0
		}

		inline bool PointOutsideOfPlane(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c,
			const Vector3& d)
		{
			const float32 signp = DotProduct(p - a, Cross(b - a, c - a)); // [AP AB AC]
			const float32 signd = DotProduct(d - a, Cross(b - a, c - a)); // [AD AB AC]
			// Points on opposite sides if expression signs are opposite
			return signp * signd < 0.0f;
		}

		inline Vector3 ClosestPtPointTetrahedron(const Vector3& p, const Vector3& a, const Vector3& b,
			const Vector3& c, const Vector3& d)
		{
			// Start out assuming point inside all halfspaces, so closest to itself
			Vector3 ClosestPoint = p;
			float32 BestSquaredDistance = 3.402823466e+38F;

			// If point outside face abc then compute closest point on abc
			if (PointOutsideOfPlane(p, a, b, c))
			{
				const Vector3 q = ClosestPointOnTriangleToPoint(p, a, b, c);
				const float32 sqDist = DotProduct(q - p, q - p);
				// Update best closest point if (squared) distance is less than current best
				if (sqDist < BestSquaredDistance) BestSquaredDistance = sqDist, ClosestPoint = q;
			}

			// Repeat test for face acd
			if (PointOutsideOfPlane(p, a, c, d))
			{
				const Vector3 q = ClosestPointOnTriangleToPoint(p, a, c, d);
				const float32 sqDist = DotProduct(q - p, q - p);
				if (sqDist < BestSquaredDistance) BestSquaredDistance = sqDist, ClosestPoint = q;
			}

			// Repeat test for face adb
			if (PointOutsideOfPlane(p, a, d, b))
			{
				const Vector3 q = ClosestPointOnTriangleToPoint(p, a, d, b);
				const float32 sqDist = DotProduct(q - p, q - p);
				if (sqDist < BestSquaredDistance) BestSquaredDistance = sqDist, ClosestPoint = q;
			}

			// Repeat test for face bdc
			if (PointOutsideOfPlane(p, b, d, c))
			{
				const Vector3 q = ClosestPointOnTriangleToPoint(p, b, d, c);
				const float32 sqDist = DotProduct(q - p, q - p);
				if (sqDist < BestSquaredDistance) BestSquaredDistance = sqDist, ClosestPoint = q;
			}

			return ClosestPoint;
		}

		//Expects unit vectors
		inline Quaternion FromTwoVectors(Vector3 u, Vector3 v) {
			float32 m = SquareRoot(2.f + 2.f * DotProduct(u, v));
			Vector3 w = (1.f / m) * Cross(u, v);
			return Quaternion(w.X(), w.Y(), w.Z(), 0.5f * m);
		}
		
		constexpr auto MakeShuffleMask(uint8 x, uint8 y, uint8 z, uint8 w) {
			return (x | (y << 2) | (z << 4) | (w << 6));
		}

		template<int MASK>
		auto VecSwizzleMask(__m128 vec) {
			return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), MASK));
		}

		template<uint8 X, uint8 Y, uint8 Z, uint8 W>
		auto VecSwizzle(__m128 vec1) { return VecSwizzleMask<MakeShuffleMask(X, Y, Z, W)>(vec1); }
		
		template<uint8 X>
		auto VecSwizzle1(__m128 vec1) { return VecSwizzleMask<MakeShuffleMask(X, X, X, X)>(vec1); }
// special swizzle
		inline auto VecSwizzle_0022(__m128 vec1) { return _mm_moveldup_ps(vec1); }
		inline auto VecSwizzle_1133(__m128 vec1) { return _mm_movehdup_ps(vec1); }

		inline auto VecShuffle(__m128 vec1, __m128 vec2) { return _mm_movelh_ps(vec1, vec2); }
		
		template<uint8 X, uint8 Y, uint8 Z, uint8 W>
		inline auto VecShuffle(__m128 vec1, __m128 vec2) {
			return _mm_shuffle_ps(vec1, vec2, MakeShuffleMask(X, Y, Z, W));
		}
		
		inline auto VecShuffle_0101(__m128 vec1, __m128 vec2) { return _mm_movelh_ps(vec1, vec2); }
		inline auto VecShuffle_2323(__m128 vec1, __m128 vec2) { return _mm_movehl_ps(vec1, vec2); }

// for row major matrix
// we use __m128 to represent 2x2 matrix as A = | A0  A1 |
//                                              | A2  A3 |
// 2x2 row major Matrix multiply A*B
		__forceinline __m128 Mat2Mul(__m128 vec1, __m128 vec2)
		{
			return
				_mm_add_ps(_mm_mul_ps(vec1, VecSwizzle<0, 3, 0, 3>(vec2)),
					_mm_mul_ps(VecSwizzle<1, 0, 3, 2>(vec1), VecSwizzle<2, 1, 2, 1>(vec2)));
		}
		// 2x2 row major Matrix adjugate multiply (A#)*B
		__forceinline __m128 Mat2AdjMul(__m128 vec1, __m128 vec2) {
			return
				_mm_sub_ps(_mm_mul_ps(VecSwizzle<3, 3, 0, 0>(vec1), vec2),
					_mm_mul_ps(VecSwizzle<1, 1, 2, 2>(vec1), VecSwizzle<2, 3, 0, 1>(vec2)));

		}
		// 2x2 row major Matrix multiply adjugate A*(B#)
		__forceinline __m128 Mat2MulAdj(__m128 vec1, __m128 vec2)
		{
			return
				_mm_sub_ps(_mm_mul_ps(vec1, VecSwizzle<3, 0, 3, 0>(vec2)),
					_mm_mul_ps(VecSwizzle<1, 0, 3, 2>(vec1), VecSwizzle<2, 1, 2, 1>(vec2)));
		}
		
		//https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html
		inline Matrix4 Inverse(const Matrix4& matrix) {
			// use block matrix method
			// A is a matrix, then i(A) or iA means inverse of A, A# (or A_ in code) means adjugate of A, |A| (or detA in code) is determinant, tr(A) is trace

			// sub matrices
			auto vec0 = _mm_load_ps(matrix[0]);
			auto vec1 = _mm_load_ps(matrix[1]);
			auto vec2 = _mm_load_ps(matrix[2]);
			auto vec3 = _mm_load_ps(matrix[3]);

			__m128 A = VecShuffle_0101(vec0, vec1);
			__m128 B = VecShuffle_2323(vec0, vec1);
			__m128 C = VecShuffle_0101(vec2, vec3);
			__m128 D = VecShuffle_2323(vec2, vec3);

			// determinant as (|A| |B| |C| |D|)
			__m128 detSub = _mm_sub_ps(
				_mm_mul_ps(VecShuffle<0, 2, 0, 2>(vec0, vec2), VecShuffle<1, 3, 1, 3>(vec1, vec3)),
				_mm_mul_ps(VecShuffle<1, 3, 1, 3>(vec0, vec2), VecShuffle<0, 2, 0, 2>(vec1, vec3))
			);
			__m128 detA = VecSwizzle1<0>(detSub); __m128 detB = VecSwizzle1<1>(detSub);
			__m128 detC = VecSwizzle1<2>(detSub); __m128 detD = VecSwizzle1<3>(detSub);

			// let iM = 1/|M| * | X  Y |
			//                  | Z  W |

			// D#C
			__m128 D_C = Mat2AdjMul(D, C);
			// A#B
			__m128 A_B = Mat2AdjMul(A, B);
			// X# = |D|A - B(D#C)
			__m128 X_ = _mm_sub_ps(_mm_mul_ps(detD, A), Mat2Mul(B, D_C));
			// W# = |A|D - C(A#B)
			__m128 W_ = _mm_sub_ps(_mm_mul_ps(detA, D), Mat2Mul(C, A_B));

			// |M| = |A|*|D| + ... (continue later)
			__m128 detM = _mm_mul_ps(detA, detD);

			// Y# = |B|C - D(A#B)#
			__m128 Y_ = _mm_sub_ps(_mm_mul_ps(detB, C), Mat2MulAdj(D, A_B));
			// Z# = |C|B - A(D#C)#
			__m128 Z_ = _mm_sub_ps(_mm_mul_ps(detC, B), Mat2MulAdj(A, D_C));

			// |M| = |A|*|D| + |B|*|C| ... (continue later)
			detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

			// tr((A#B)(D#C))
			__m128 tr = _mm_mul_ps(A_B, VecSwizzle<0, 2, 1, 3>(D_C));
			tr = _mm_hadd_ps(tr, tr);
			tr = _mm_hadd_ps(tr, tr);
			// |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C)
			detM = _mm_sub_ps(detM, tr);

			const __m128 adjSignMask = _mm_setr_ps(1.f, -1.f, -1.f, 1.f);
			// (1/|M|, -1/|M|, -1/|M|, 1/|M|)
			__m128 rDetM = _mm_div_ps(adjSignMask, detM);

			X_ = _mm_mul_ps(X_, rDetM);
			Y_ = _mm_mul_ps(Y_, rDetM);
			Z_ = _mm_mul_ps(Z_, rDetM);
			W_ = _mm_mul_ps(W_, rDetM);

			Matrix4 result;

			// apply adjugate and store, here we combine adjugate shuffle and store shuffle
			_mm_store_ps(result[0], VecShuffle<3, 1, 3, 1>(X_, Y_)); _mm_store_ps(result[1], VecShuffle<2, 0, 2, 0>(X_, Y_));
			_mm_store_ps(result[2], VecShuffle<3, 1, 3, 1>(Z_, W_)); _mm_store_ps(result[3], VecShuffle<2, 0, 2, 0>(Z_, W_));

			return result;
		}

		template<typename T1, typename T2, typename MF, typename SF>
		inline void MultiplesFor(const Range<T1*> range1, const Range<T2*> range2, const uint64 multiple, const MF& multiplesFunction, const SF& singlesFunction)
		{
			GTSL_ASSERT(range1.ElementCount() == range2.ElementCount(), "Element count is not equal!")
			
			uint64 quotient, remainder;
			RoundDown(range1.ElementCount(), multiple, quotient, remainder);

			uint64 i = 0;
			for (auto begin = range1.begin(); begin != range1.end() - remainder; begin += multiple) { multiplesFunction(range1.begin() + i, range2.begin() + i); i += multiple; }
			
			i = 0;
			for (auto begin = range1.end() - remainder; begin != range1.end(); ++begin) { singlesFunction(range1.begin() + i, range2.begin() + i); i += multiple; }
		}
	}
	
	// +  float32
// += float32
// +  type
// += type
// -  float32
// -= float32
// -  type
// -= type
// *  float32
// *= float32
// *  type
// *= type
// /  float32
// /= float32
// /  type
// /= type
}
