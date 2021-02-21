#pragma once

#include <immintrin.h>

#include "GTSL/Core.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4.h"

#include "Transform3.h"
#include "Plane.h"
#include "Rotator.h"
#include "GTSL/Assert.h"
#include "GTSL/Range.h"
#include "GTSL/Window.h"

namespace GTSL
{
	union FloatintPoint
	{
		FloatintPoint(float32 f) : Float(f) {}
		FloatintPoint(int32 i) : Int(i) {}

		float32 Float; int32 Int;
	};
	
	/**
	 * \brief Provides most of the same methods of math but usually skips input checking for non valid inputs, NaN, etc. Useful when working in conditions which assure you certain
	 * validity of inputs. This functions are faster than those of Math.
	 */
	class AdvancedMath
	{
	public:
		/**
		 * \brief Computes a square root via IEEE754 bit hacking which is faster but less accurate(6% deviation). Accepts only positive values, undefined for negative values or NaN.
		 * \param x Number to compute the square root of
		 * \return The square root of x
		 */
		static float32 FastImpreciseSqrt(const float32 x)
		{
			return FloatintPoint((FloatintPoint(x).Int >> 1) + (FloatintPoint(1.0f).Int >> 1)).Float; // aprox
		}

		/**
		 * \brief Computes a more precise square root via IEEE754 bit hacking (0.04% deviation). Accepts only positive values, undefined for negative values or NaN.
		 * \param x Number to compute the square root of
		 * \return The square root of x
		 */
		static float32 FastSqrt(const float32 x)
		{
			auto y = FastImpreciseSqrt(x);
			return (y * y + x) / (2.0f * y); //Newton-Rhapson iteration for increased precision
		}

		template<typename T>
		static T SquareRoot(const T a)
		{
			constexpr T error = 0.000001; //define the precision of your result
			float s = a;

			T aS;

			do
			{
				aS = a / s;
				s = (s + aS) * 0.5;
			}
			while (s - aS > error); //loop until precision satisfied

			return s;
		}
	
	private:
	};
	
	class Math
	{
	public:
		
		static float32 StraightRaise(const float32 A, const uint8 Times)
		{
			float32 Result = A;

			for (uint8 i = 0; i < Times - 1; i++)
			{
				Result *= A;
			}

			return Result;
		}

		static constexpr float64 PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286;
		static constexpr float64 e = 2.718281828459045235360287471352662497757247093699959574966967627724076630353;
		//static constexpr float32 Epsilon = 2.718281828459045235360287471352662497757247093699959574966967627724076630353;

		static int64 Random()
		{
			static thread_local int64 x = 123456789, y = -362436069, z = 521288629;

			//period 2^96-1
			
			int64 t{ 0 };
			x ^= x << 16; x ^= x >> 5; x ^= x << 1;

			t = x; x = y; y = z; z = t ^ x ^ y;

			return z;
		}

		static int64 Random(const int64 min, const int64 max) { return Random() % (max - min + 1) + min; }

		static float64 RandomFloat() { return Random() * 0.8123495678; }

		//STATIC

		static int32 Floor(const float32 A) { return static_cast<int32>(A - (static_cast<int32>(A) % 1)); }

		static float32 Modulo(const float32 A, const float32 B)
		{
			const float32 C = A / B;
			return (C - static_cast<float32>(static_cast<int32>(C))) * B;
		}

		static uint32 Fact(const int8 A)
		{
			uint8 result = 1;

			for (uint8 i = 1; i < A + 1; ++i)
			{
				result *= i + 1;
			}

			return result;
		}

#if (!_WIN64)
		static void RoundDown(const uint64 x, const uint64 multiple, uint64& quotient, uint64& remainder) { const uint64 rem = x % multiple; remainder = rem; quotient = x - rem; }
#endif
#if (_WIN64)
		static void RoundDown(const uint64 x, const uint32 multiple, uint32& quotient, uint32& remainder) { quotient = _udiv64(x, multiple, &remainder); }
#endif

		static bool IsPowerOfTwo(const uint64 n) { return (n & (n - 1)) == 0; }
		
		static uint64 RoundUpByPowerOf2(const uint64 n, const uint64 powerOfTwo)
		{
			GTSL_ASSERT(IsPowerOfTwo(powerOfTwo), "Is not multiple of two!")
			return n + (powerOfTwo - 1) & ~(powerOfTwo - 1);
		}

		static uint32 RoundUpByPowerOf2(const uint32 n, const uint32 powerOfTwo)
		{
			GTSL_ASSERT(IsPowerOfTwo(powerOfTwo), "Is not multiple of two!")
			return n + (powerOfTwo - 1) & ~(powerOfTwo - 1);
		}

		static uint16 RoundUpByPowerOf2(const uint16 n, const uint16 powerOfTwo)
		{
			GTSL_ASSERT(IsPowerOfTwo(powerOfTwo), "Is not multiple of two!")
			return n + (powerOfTwo - 1) & ~(powerOfTwo - 1);
		}
		
		static uint64 RoundUp(const uint64 number, const uint32 multiple)
		{
			const uint64 m_m_1 = multiple - 1, sum = number + m_m_1; return sum - sum % multiple;
		}
		/**
		 * \brief Returns x to the y.
		 * \param x number
		 * \param y exponent
		 * \return x ^ y
		 */
		static float32 Power(float32 x, float32 y);

		/**
		 * \brief Returns the base 10 logarithm of x.
		 * \param x number
		 * \return Base 10 logarithm of x
		 */
		static float32 Log10(float32 x);

		/**
		 * \brief Returns the sine of an angle.
		 * \param radians Angle in radians.
		 * \return Sine of radians
		 */
		static float32 Sine(float32 radians);

		/**
		 * \brief Returns the sine of an angle.
		 * \param radians Angle in radians.
		 * \return Sine of radians
		 */
		static float64 Sine(float64 radians);

		/**
		* \brief Returns the cosine of an angle.
		* \param radians Angle in radians.
		* \return Cosine of radians
		*/
		static float32 Cosine(float32 radians);

		/**
		* \brief Returns the cosine of an angle.
		* \param radians Angle in radians.
		* \return Cosine of radians
		*/
		static float64 Cosine(float64 radians);

		/**
		* \brief Returns the tangent of an angle.
		* \param radians Angle in radians.
		* \return Tangent of radians
		*/
		static float32 Tangent(float32 radians);

		/**
		* \brief Returns the tangent of an angle.
		* \param radians Angle in radians.
		* \return Tangent of radians
		*/
		static float64 Tangent(float64 radians);

		/**
		* \brief Returns the arcsine of A in radians.
		* \param A value
		* \return Radians of A
		*/
		static float32 ArcSine(float32 A);

		/**
		* \brief Returns the arccosine of A in radians.
		* \param A value
		* \return Radians of A
		*/
		static float32 ArcCosine(float32 A);

		/**
		* \brief Returns the arctangent of A in radians.
		* \param A value
		* \return Radians of A
		*/
		static float32 ArcTangent(float32 A);

		/**
		* \brief Returns the arctangent of Y / X in degrees.
		* \return Degrees of Y / X
		*/
		static float32 ArcTan2(float32 X, float32 Y);

		/**
		 * \brief Computes a square root via IEEE754 bit hacking which is faster but less accurate.
		 * \param x Number to compute the square root of
		 * \return The square root of x
		 */
		static float32 FastSqrt(const float32 x)
		{
			if (x > 0.0f) { return AdvancedMath::FastSqrt(x); }
			return 0.0f;
		}

		template<typename T>
		static T Square(const T a) { return a * a; }
		
		//////////////////////////////////////////////////////////////
		//						SCALAR MATH							//
		//////////////////////////////////////////////////////////////

		template<typename T>
		static void MinMax(T a, T b, T& min, T& max)
		{
			if (a < b) { min = a; max = b; }
			else { max = a; min = b; }
		}

		template<>
		static void MinMax(Vector2 a, Vector2 b, Vector2& min, Vector2& max)
		{
			MinMax(a.X(), b.X(), min.X(), max.X()); MinMax(a.Y(), b.Y(), min.Y(), max.Y());
		}

		//Returns 1 if A is bigger than 0. 0 if A is equal to 0. and -1 if A is less than 0.
		static int8 Sign(const int64 A)
		{
			if (A > 0) { return 1; } if (A < 0) { return -1; } return 0;
		}

		//Returns 1 if A is bigger than 0. 0 if A is equal to 0. and -1 if A is less than 0.
		static int8 Sign(const float32 A)
		{
			if (A > 0.0f)
			{
				return 1;
			}
			if (A < 0.0f)
			{
				return -1;
			}

			return 0;
		}

		//Mixes A and B by the specified values, Where Alpha 0 returns A and Alpha 1 returns B.
		static float32 Lerp(const float32 A, const float32 B, const float32 Alpha)
		{
			return A + Alpha * (B - A);
		}

		static Vector2 Lerp(const Vector2 a, const Vector2 b, const float32 alpha)
		{
			return Vector2(Lerp(a.X(), b.X(), alpha), Lerp(a.Y(), b.Y(), alpha));
		}

		static float32 Square(const float32 a) { return a * a; }

		//Interpolates from Current to Target, returns Current + an amount determined by the InterpSpeed.
		static float32 FInterp(const float32 Target, const float32 Current, const float32 DT, const float32 InterpSpeed)
		{
			return (Target - Current) * DT * InterpSpeed + Current;
		}

		static uint32 InvertRange(const uint32 a, const uint32 max)
		{
			return max - a;
		}

		static float32 InvertRange(const float32 a, const float32 max)
		{
			return max - a;
		}
		
		static float32 MapToRange(const float32 x, const float32 inMin, const float32 inMax, const float32 outMin, const float32 outMax)
		{
			if ((inMax - inMin) != 0.0f)
			{
				GTSL_ASSERT(x >= inMin && x <= inMax, "Not in range");
				return (x - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
			}

			return x;
		}

		static Vector2 MapToRange(const GTSL::Vector2 A, const Vector2 matrixin, const Vector2 matrixax, const Vector2 OutMin, const Vector2 OutMax)
		{
			return Vector2(MapToRange(A.X(), matrixin.X(), matrixax.X(), OutMin.X(), OutMax.X()), MapToRange(A.Y(), matrixin.Y(), matrixax.Y(), OutMin.Y(), OutMax.Y()));
		}

		static float32 MapToRangeZeroToOne(const float32 a, const float32 inMax, const float32 outMax) { return a / (inMax / outMax); }

		static float32 SquareRoot(const float32 a)
		{
			if (a > 0.0f)
			{
				return AdvancedMath::SquareRoot(a);
			}

			return 0.0f;
		}

		static float64 SquareRoot(const float64 a)
		{
			if (a > 0.0)
			{
				return AdvancedMath::SquareRoot(a);
			}

			return 0.0;
		}

		static float32 Root(const float32 a, const float32 root) { return Power(a, 1.0f / root); }

		static uint32 Abs(const int32 a) { return uint32(a < 0.0f ? -a : a); }

		static uint64 Abs(const int64 a) { return uint64(a < 0.0f ? -a : a); }

		static float32 Abs(const float32 a) { return a < 0.0f ? -a : a; }

		template<typename T>
		static T Limit(const T a, const T max) { return a > max ? max : a; }
		
		template <typename T>
		static T Min(const T& A, const T& B) { return (A < B) ? A : B; }

		template <typename T>
		static T Max(const T& A, const T& B) { return (A > B) ? A : B; }

		static Vector2 Min(const Vector2 a, const Vector2 b) { return Vector2(Min(a.X(), b.X()), Min(a.Y(), b.Y())); }
		static Vector2 Max(const Vector2 a, const Vector2 b) { return Vector2(Max(a.X(), b.X()), Max(a.Y(), b.Y())); }
		
		static Vector3 Min(const Vector3 a, const Vector3 b) { return Vector3(Min(a.X(), b.X()), Min(a.Y(), b.Y()), Min(a.Z(), b.Z())); }
		static Vector3 Max(const Vector3 a, const Vector3 b) { return Vector3(Max(a.X(), b.X()), Max(a.Y(), b.Y()), Max(a.Z(), b.Z())); }

		static Vector4 Min(const Vector4 a, const Vector4 b) { return Vector4(Min(a.X(), b.X()), Min(a.Y(), b.Y()), Min(a.Z(), b.Z()), Min(a.W(), b.W())); }
		static Vector4 Max(const Vector4 a, const Vector4 b) { return Vector4(Max(a.X(), b.X()), Max(a.Y(), b.Y()), Max(a.Z(), b.Z()), Max(a.W(), b.W())); }
		
		/**
		 * \brief Returns degrees converted to radians.
		 * \param degrees degrees to convert
		 * \return Degrees as radians
		 */
		static float32 DegreesToRadians(const float32 degrees) { return degrees * static_cast<float32>(PI / 180.0); }

		/**
		 * \brief Returns degrees converted to radians.
		 * \param degrees degrees to convert
		 * \return Degrees as radians
		 */
		static float64 DegreesToRadians(const float64 degrees) { return degrees * (PI / 180.0); }

		/**
		 * \brief Returns Radians converted to degrees.
		 * \param radians radians to convert.
		 * \return Radians as degrees.
		 */
		static float32 RadiansToDegrees(const float32 radians) { return radians * static_cast<float32>(180.0 / PI); }

		/**
		 * \brief Returns Radians converted to degrees.
		 * \param radians radians to convert.
		 * \return Radians as degrees.
		 */
		static float64 RadiansToDegrees(const float64 radians) { return radians * (180.0 / PI); }

		//////////////////////////////////////////////////////////////
		//						VECTOR MATH							//
		//////////////////////////////////////////////////////////////

		//Calculates the length of a 2D vector.
		static float32 Length(const Vector2 a) { return SquareRoot(LengthSquared(a)); }
		static float32 Length(const Vector2 a, const Vector2 b) { return SquareRoot(LengthSquared(a, b)); }
		static float32 Length(const Vector3 a) { return SquareRoot(LengthSquared(a)); }
		static float32 Length(const Vector3 a, const Vector3 b) { return SquareRoot(LengthSquared(a, b)); }
		static float32 Length(const Vector4 a) { return SquareRoot(LengthSquared(a)); }
		static float32 Length(const Vector4 a, const Vector4 b)	{ return SquareRoot(LengthSquared(a, b)); }

		static float32 LengthSquared(const Vector2 a) { return a.X() * a.X() + a.Y() * a.Y(); }
		static float32 LengthSquared(const Vector2 a, const Vector2 b) { return LengthSquared(b - a); }
		static float32 LengthSquared(const Vector3 a) { return a.X() * a.X() + a.Y() * a.Y() + a.Z() * a.Z(); }
		static float32 LengthSquared(const Vector3 a, const Vector3 b) { return LengthSquared(b - a); }
		static float32 LengthSquared(const Vector4 a) { return a.X() * a.X() + a.Y() * a.Y() + a.Z() * a.Z() + a.W() * a.W(); }
		static float32 LengthSquared(const Vector4 a, const Vector4 b) { return LengthSquared(b - a); }

		static Vector2 Normalized(const Vector2& a)
		{
			auto length = Length(a); if (length == 0.0f) { return a; }
			length = 1.0f / length;
			return Vector2(a.X() * length, a.Y() * length);
		}
		static void Normalize(Vector2& a)
		{
			auto length = Length(a);
			if (length == 0.0f) { return; }
			length = 1.0f / length;
			a.X() *= length; a.Y() *= length;
		}
		static Vector3 Normalized(const Vector3& a)
		{
			auto length = Length(a); if (length == 0.0f) { return a; }
			length = 1.0f / length;
			return Vector3(a.X() * length, a.Y() * length, a.Z() * length);
		}
		static void Normalize(Vector3& a)
		{
			auto length = Length(a);
			if (length == 0.0f) { return; }
			length = 1.0f / length;
			a.X() *= length; a.Y() *= length; a.Z() *= length;
		}
		static Vector4 Normalized(const Vector4& a)
		{
			auto length = Length(a); if (length == 0.0f) { return a; }
			length = 1.0f / length;
			return Vector4(a.X() * length, a.Y() * length, a.Z() * length, a.W() * length);
		}
		static void Normalize(Vector4& a)
		{
			auto length = Length(a);
			if (length == 0.0f) { return; }
			length = 1.0f / length;
			a.X() *= length; a.Y() *= length; a.Z() *= length; a.W() *= length;
		}
		static void Normalize(Quaternion& a)
		{
			auto length = Length(a);
			if (length == 0.0f) { a.X() = 1.0f; return; }
			length = 1.0f / length;
			a.X() *= length; a.Y() *= length; a.Z() *= length; a.W() *= length;
		}

		static float32 DotProduct(const Vector2 a, const Vector2 b) { return a.X() * b.X() + a.Y() * b.Y(); }
		static float32 DotProduct(const Vector3& a, const Vector3& b) { return a.X() * a.X() + a.Y() * a.Y() + a.Z() * a.Z(); }
		static float32 DotProduct(const Vector4& a, const Vector4& b) { return a.X() * a.X() + a.Y() * a.Y() + a.Z() * a.Z() + a.W() * a.W(); }

		static Vector3 Cross(const Vector3& a, const Vector3& b) { return Vector3(a.Y() * b.Z() - a.Z() * b.Y(), a.Z() * b.X() - a.X() * b.Z(), a.X() * b.Y() - a.Y() * b.X()); }

		static Vector2 Abs(const Vector2 a) { return Vector2(Abs(a.X()), Abs(a.Y())); }
		static Vector3 Abs(const Vector3 a) { return Vector3(Abs(a.X()), Abs(a.Y()), Abs(a.Z())); }
		static Vector4 Abs(const Vector4 a) { return Vector4(Abs(a.X()), Abs(a.Y()), Abs(a.Z()), Abs(a.W())); }

		static Vector2 Negated(const Vector2& Vec)
		{
			Vector2 Result;

			Result.X() = -Vec.X();
			Result.Y() = -Vec.Y();

			return Result;
		}

		static void Negate(Vector2& Vec) { Vec.X() = -Vec.X(); Vec.Y() = -Vec.Y(); }

		static Vector3 Negated(const Vector3& Vec)
		{
			Vector3 Result;

			Result.X() = -Vec.X();
			Result.Y() = -Vec.Y();
			Result.Z() = -Vec.Z();

			return Result;
		}

		static void Negate(Vector3& Vec) { Vec.X() = -Vec.X(); Vec.Y() = -Vec.Y(); Vec.Z() = -Vec.Z(); }

		static Vector4 Negated(const Vector4& Vec)
		{
			Vector4 Result;

			Result.X() = -Vec.X();
			Result.Y() = -Vec.Y();
			Result.Z() = -Vec.Z();
			Result.W() = -Vec.W();

			return Result;
		}

		static void Negate(Vector4& a)
		{
			a.X() = -a.X();
			a.Y() = -a.Y();
			a.Z() = -a.Z();
			a.W() = -a.W();
		}

		//////////////////////////////////////////////////////////////
		//						QUATERNION MATH						//
		//////////////////////////////////////////////////////////////

		static Quaternion Conjugated(const Quaternion& a) { return Quaternion(-a.X(), -a.Y(), -a.Z(), a.W()); }

		static void Conjugate(Quaternion& a) { a.X() = -a.X(); a.Y() = -a.Y(); a.Z() = -a.Z(); }


		//////////////////////////////////////////////////////////////
		//						LOGIC								//
		//////////////////////////////////////////////////////////////

		static bool IsNearlyEqual(const float32 A, const float32 Target, const float32 Tolerance)
		{
			return (A >= Target - Tolerance) && (A <= Target + Tolerance);
		}

		static bool IsInRange(const float32 A, const float32 Min, const float32 Max)
		{
			return (A >= Min) && (A <= Max);
		}

		static bool IsVectorNearlyEqual(const Vector2& A, const Vector2& Target, const float32 Tolerance)
		{
			return IsNearlyEqual(A.X(), Target.X(), Tolerance) && IsNearlyEqual(A.Y(), Target.Y(), Tolerance);
		}

		static bool IsVectorNearlyEqual(const Vector3& A, const Vector3& Target, const float32 Tolerance)
		{
			if (IsNearlyEqual(A.X(), Target.X(), Tolerance))
			{
				if (IsNearlyEqual(A.Y(), Target.Y(), Tolerance))
				{
					if (IsNearlyEqual(A.Z(), Target.Z(), Tolerance))
					{
						return true;
					}
				}
			}

			return false;
		}

		static bool IsVectorNearlyEqual(const Vector4& a, const Vector4& b, const float32 tolerance)
		{
			if (IsNearlyEqual(a.X(), b.X(), tolerance)) {
				if (IsNearlyEqual(a.Y(), b.Y(), tolerance)) {
					if (IsNearlyEqual(a.Z(), b.Z(), tolerance)) {
						if (IsNearlyEqual(a.W(), b.W(), tolerance)) {
							return true;
						}
					}
				}
			}

			return false;
		}

		static bool AreVectorComponentsGreater(const Vector3& A, const Vector3& B)
		{
			return A.X() > B.X() && A.Y() > B.Y() && A.Z() > B.Z();
		}

		static bool PointInBox(Vector2 min, Vector2 max, Vector2 p) { return p.X() >= min.X() && p.X() <= max.X() && p.Y() >= min.Y() && p.Y() <= max.Y(); }

		static bool PointInBoxProjection(Vector2 min, Vector2 max, Vector2 p) { return p.X() >= min.X() && p.X() <= max.X() || p.Y() >= min.Y() && p.Y() <= max.Y(); }

		//////////////////////////////////////////////////////////////
		//						MATRIX MATH							//
		//////////////////////////////////////////////////////////////

		//Creates a translation matrix.
		static Matrix4 Translation(const Vector3& Vector)
		{
			Matrix4 result(1);

			result(0, 3) = Vector.X();
			result(1, 3) = Vector.Y();
			result(2, 3) = Vector.Z();

			return result;
		}

		//Modifies the given matrix to make it a translation matrix.
		static void Translate(Matrix4& mA, const Vector3& vA)
		{
			const auto translation = Translation(vA);
			mA *= translation;
		}

		static Matrix4 NormalToRotation(Vector3 normal)
		{
			// Find a vector in the plane
			Vector3 tangent0 = Cross(normal, Vector3(1, 0, 0));
			if (DotProduct(tangent0, tangent0) < 0.001)
				tangent0 = Cross(normal, Vector3(0, 1, 0));
			Normalize(tangent0);
			// Find another vector in the plane
			const Vector3 tangent1 = Normalized(Cross(normal, tangent0));
			return Matrix4(tangent0.X(), tangent0.Y(), tangent0.Z(), 0.0f, tangent1.X(), tangent1.Y(), tangent1.Z(), 0.0f, normal.X(), normal.Y(), normal.Z(), 0.0f, 0, 0, 0, 0);
		}

		static void Rotate(Matrix4& A, const Quaternion& Q)
		{
			const auto rotation = Rotation(Q);

			A *= rotation;
		}

		//Returns point colinearity to a line defined by two points.
		// +0 indicates point is to the right
		// 0 indicates point is on the line
		// -0 indicates point is to the left
		static float32 TestPointToLineSide(const GTSL::Vector2 a, const GTSL::Vector2 b, const GTSL::Vector2 p)
		{
			return ((a.X() - b.X()) * (p.Y() - b.Y()) - (a.Y() - b.Y()) * (p.X() - b.X()));
		};

		static Vector3 SphericalCoordinatesToCartesianCoordinates(const Vector2& sphericalCoordinates)
		{
			const auto cy = Cosine(sphericalCoordinates.Y());

			return Vector3(cy * Sine(sphericalCoordinates.X()), Sine(sphericalCoordinates.Y()),	cy * Cosine(sphericalCoordinates.X()));
		}

		static Vector3 RotatorToNormalVector(const Rotator& rotator)
		{
			const auto x = Cosine(rotator.Y) * Cosine(rotator.X);
			const auto y = Sine(rotator.Y) * Cosine(rotator.X);
			const auto z = Sine(rotator.X);

			return Vector3(x, y, z);
		}

		static Quaternion RotatorToQuaternion(const Rotator& rotator)
		{
			// Abbreviations for the various angular functions
			const auto cy = Cosine(rotator.Y * 0.5f);
			const auto sy = Sine(rotator.Y * 0.5f);
			const auto cp = Cosine(rotator.X * 0.5f);
			const auto sp = Sine(rotator.X * 0.5f);
			const auto cr = Cosine(rotator.Z * 0.5f);
			const auto sr = Sine(rotator.Z * 0.5f);

			Quaternion result;
			result.X() = sy * cp * sr + cy * sp * cr;
			result.Y() = sy * cp * cr - cy * sp * sr;
			result.Z() = cy * cp * sr - sy * sp * cr;
			result.W() = cy * cp * cr + sy * sp * sr;

			return result;
		}

		static Matrix4 Rotation(const Quaternion& A)
		{
			Matrix4 result(1);

			const auto xx = A.X() * A.X();
			const auto xy = A.X() * A.Y();
			const auto xz = A.X() * A.Z();
			const auto xw = A.X() * A.W();
			const auto yy = A.Y() * A.Y();
			const auto yz = A.Y() * A.Z();
			const auto yw = A.Y() * A.W();
			const auto zz = A.Z() * A.Z();
			const auto zw = A.Z() * A.W();

			result(0, 0) = 1 - 2 * (yy + zz);
			result(0, 1) = 2 * (xy - zw);
			result(0, 2) = 2 * (xz + yw);
			result(1, 0) = 2 * (xy + zw);
			result(1, 1) = 1 - 2 * (xx + zz);
			result(1, 2) = 2 * (yz - xw);
			result(2, 0) = 2 * (xz - yw);
			result(2, 1) = 2 * (yz + xw);
			result(2, 2) = 1 - 2 * (xx + yy);
			result(0, 3) = result(1, 3) = result(2, 3) = result(3, 0) = result(3, 1) = result(3, 2) = 0;
			result(3, 3) = 1;

			return result;
		}

		static Matrix4 Rotation(const Vector3& A, float32 angle)
		{
			Matrix4 result(1);

			const float32 c = Cosine(angle);
			const float32 s = Sine(angle);
			const auto t = 1.0f - c;
			const float32 xx = A.X() * A.X();
			const float32 xy = A.X() * A.Y();
			const float32 xz = A.X() * A.Z();
			const float32 yy = A.Y() * A.Y();
			const float32 yz = A.Y() * A.Z();
			const float32 zz = A.Z() * A.Z();

			// build rotation matrix
			result(0, 0) = c + xx * t;
			result(1, 1) = c + yy * t;
			result(2, 2) = c + zz * t;

			auto tmp1 = xy * t;
			auto tmp2 = A.Z() * s;
			
			result(1, 0) = tmp1 + tmp2;
			result(0, 1) = tmp1 - tmp2;

			tmp1 = xz * t;
			tmp2 = A.Y() * s;
			
			result(2, 0) = tmp1 - tmp2;
			result(0, 2) = tmp1 + tmp2;

			tmp1 = yz * t;
			tmp2 = A.X() * s;
			
			result(2, 1) = tmp1 + tmp2;
			result(1, 2) = tmp1 - tmp2;

			return result;
		}

		static void Scale(Matrix4& A, const Vector3& B)
		{
			const auto scaling = Scaling(B);

			A *= scaling;
		}

		static Matrix4 Scaling(const Vector3& A)
		{
			Matrix4 Result(1.0f);

			Result[0] = A.X();
			Result[5] = A.Y();
			Result[10] = A.Z();

			return Result;
		}

		static Matrix4 Transformation(const Transform3& _A)
		{
			Matrix4 Return;
			Translate(Return, _A.Position);
			//Rotate(Return, _A.Rotation);
			Scale(Return, _A.Scale);
			return Return;
		}

		static void Transform(Matrix4& _A, Transform3& _B)
		{
			Translate(_A, _B.Position);
			//Rotate(_A, _B.Rotation);
			Scale(_A, _B.Scale);
		}

		static void BuildPerspectiveMatrix(Matrix4& matrix, const float32 fov, const float32 aspectRatio, const float32 nearPlane, const float32 farPlane)
		{
			//Tangent of half the vertical view angle.
			const auto f = 1 / Tangent(fov * 0.5f);

			const auto far_m_near = farPlane - nearPlane;

			//Zero to one
			//Left handed

			matrix(0, 0) = f / aspectRatio;

			matrix(1, 1) = -f;

			matrix(2, 2) = -((farPlane + nearPlane) / far_m_near);
			matrix(2, 3) = -((2.f * farPlane * nearPlane) / far_m_near);

			matrix(3, 2) = -1.0f;
		}

		static void MakeOrthoMatrix(Matrix4& matrix, const float32 right, const float32 left, const float32 top, const float32 bottom, const float32 nearPlane, const float32 farPlane)
		{
			//Zero to one
			//Left handed

			matrix(0, 0) = static_cast<float32>(2) / (right - left);
			matrix(1, 1) = static_cast<float32>(2) / (top - bottom);
			matrix(2, 2) = static_cast<float32>(1) / (farPlane - nearPlane);
			matrix(3, 0) = -(right + left) / (right - left);
			matrix(3, 1) = -(top + bottom) / (top - bottom);
			matrix(3, 2) = -nearPlane / (farPlane - nearPlane);
		}

		template<typename T>
		static T Clamp(T a, T min, T max) { return a > max ? max : (a < min ? min : a); }

		static Vector3 ClosestPointOnPlane(const Vector3& point, const Plane& plane)
		{
			const float32 T = (DotProduct(plane.Normal, point) - plane.D) / DotProduct(plane.Normal, plane.Normal);
			return point - plane.Normal * T;
		}

		static float64 DistanceFromPointToPlane(const Vector3& point, const Plane& plane)
		{
			// return Dot(q, p.n) - p.d; if plane equation normalized (||p.n||==1)
			return (DotProduct(plane.Normal, point) - plane.D) / DotProduct(plane.Normal, plane.Normal);
		}

		static Vector2 ClosestPointOnLineToPoint(const Vector2 a, const Vector2 b, const Vector2 p)
		{
			const auto m = b - a;
			const auto t0 = DotProduct(m, p - a) / DotProduct(m, m);
			return a + m * t0;
		}

		static Vector2 ClosestPointOnLineSegmentToPoint(const Vector2 a, const Vector2 b, const Vector2 p)
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
		
		static Vector2 ClosestPointOnLineSegmentToPoint(const Vector2 a, const Vector2 b, const Vector2 p, float32& isOnLine)
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

		static Vector3 ClosestPointOnLineSegmentToPoint(const Vector3& a, const Vector3& b, const Vector3& p)
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

		static float64 SquaredDistancePointToSegment(const Vector3& _A, const Vector3& _B, const Vector3& _C)
		{
			const Vector3 AB = _B - _A;
			const Vector3 AC = _C - _A;
			const Vector3 BC = _C - _B;
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
		static void Barycentric(Vector2 a, Vector2 b, Vector2 c, Vector2 p, float32& s, float32& t, float32& u)
		{
			Vector2 v0 = b - a, v1 = c - a, v2 = p - a;
			float32 den = v0.X() * v1.Y() - v1.X() * v0.Y();
			s = (v2.X() * v1.Y() - v1.X() * v2.Y()) / den;	
			t = (v0.X() * v2.Y() - v2.X() * v0.Y()) / den;
			u = 1.0f - s - t;
		}

		static Vector3 ClosestPointOnTriangleToPoint(const Vector3& _A, const Vector3& _P1, const Vector3& _P2, const Vector3& _P3)
		{
			// Check if P in vertex region outside A
			const Vector3 AP = _A - _P1;
			const Vector3 AB = _P2 - _P1;
			const Vector3 AC = _P3 - _P1;

			const float32 D1 = DotProduct(AB, AP);
			const float32 D2 = DotProduct(AC, AP);
			if (D1 <= 0.0f && D2 <= 0.0f) return _P1; // barycentric coordinates (1,0,0)

			// Check if P in vertex region outside B
			const Vector3 BP = _A - _P2;
			const float32 D3 = DotProduct(AB, BP);
			const float32 D4 = DotProduct(AC, BP);
			if (D3 >= 0.0f && D4 <= D3) return _P2; // barycentric coordinates (0,1,0)

			// Check if P in edge region of AB, if so return projection of P onto AB
			const float32 VC = D1 * D4 - D3 * D2;
			if (VC <= 0.0f && D1 >= 0.0f && D3 <= 0.0f)
			{
				const float32 V = D1 / (D1 - D3);
				return _P1 + AB * V; // barycentric coordinates (1-v,v,0)
			}

			// Check if P in vertex region outside C
			const Vector3 CP = _A - _P3;
			const float32 D5 = DotProduct(AB, CP);
			const float32 D6 = DotProduct(AC, CP);
			if (D6 >= 0.0f && D5 <= D6) return _P3; // barycentric coordinates (0,0,1)

			// Check if P in edge region of AC, if so return projection of P onto AC
			const float32 VB = D5 * D2 - D1 * D6;
			if (VB <= 0.0f && D2 >= 0.0f && D6 <= 0.0f)
			{
				const float32 W = D2 / (D2 - D6);
				return _P1 + AC * W; // barycentric coordinates (1-w,0,w)
			}

			// Check if P in edge region of BC, if so return projection of P onto BC
			const float32 VA = D3 * D6 - D5 * D4;
			if (VA <= 0.0f && (D4 - D3) >= 0.0f && (D5 - D6) >= 0.0f)
			{
				const float32 W = (D4 - D3) / ((D4 - D3) + (D5 - D6));
				return _P2 + (_P3 - _P2) * W; // barycentric coordinates (0,1-w,w)
			}

			// P inside face region. Compute Q through its barycentric coordinates (u,v,w)
			const float32 Denom = 1.0f / (VA + VB + VC);
			const float32 V = VB * Denom;
			const float32 W = VC * Denom;
			return _P1 + AB * V + AC * W; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
		}

		static bool PointOutsideOfPlane(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
		{
			return DotProduct(p - a, Cross(b - a, c - a)) >= 0.0f; // [AP AB AC] >= 0
		}

		static bool PointOutsideOfPlane(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c,
			const Vector3& d)
		{
			const float32 signp = DotProduct(p - a, Cross(b - a, c - a)); // [AP AB AC]
			const float32 signd = DotProduct(d - a, Cross(b - a, c - a)); // [AD AB AC]
			// Points on opposite sides if expression signs are opposite
			return signp * signd < 0.0f;
		}

		static Vector3 ClosestPtPointTetrahedron(const Vector3& p, const Vector3& a, const Vector3& b,
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

		static void SinCos(float32* sp, float32* cp, const float32 degrees)
		{
			*sp = Sine(degrees);
			*cp = Cosine(degrees);
		}

		//https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html
		static Matrix4 Inverse(const Matrix4& matrix);

		template<typename T1, typename T2, typename MF, typename SF>
		static void MultiplesFor(const Range<T1*> range1, const Range<T2*> range2, const uint64 multiple, const MF& multiplesFunction, const SF& singlesFunction)
		{
			GTSL_ASSERT(range1.ElementCount() == range2.ElementCount(), "Element count is not equal!")
			
			uint64 quotient, remainder;
			RoundDown(range1.ElementCount(), multiple, quotient, remainder);

			uint64 i = 0;
			for (auto begin = range1.begin(); begin != range1.end() - remainder; begin += multiple) { multiplesFunction(range1.begin() + i, range2.begin() + i); i += multiple; }
			
			i = 0;
			for (auto begin = range1.end() - remainder; begin != range1.end(); ++begin) { singlesFunction(range1.begin() + i, range2.begin() + i); i += multiple; }
		}
	};

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
