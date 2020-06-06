#pragma once

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
#include "GTSL/Ranger.h"

namespace GTSL
{
	class Math
	{
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

		static float32 StraightRaise(const float32 A, const uint8 Times)
		{
			float32 Result = A;

			for (uint8 i = 0; i < Times - 1; i++)
			{
				Result *= A;
			}

			return Result;
		}

	public:
		static constexpr float64 PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286;
		static constexpr float64 e = 2.718281828459045235360287471352662497757247093699959574966967627724076630353;

		static int64 Random()
		{
			static int64 x = 123456789, y = -362436069, z = 521288629;

			//period 2^96-1
			
			int64 t{ 0 };
			x ^= x << 16; x ^= x >> 5; x ^= x << 1;

			t = x; x = y; y = z; z = t ^ x ^ y;

			return z;
		}

		static int64 Random(const int64 min, const int64 max) { return Random() % (max - min + 1) + min; }

		static float64 fRandom() { return Random() * 0.8123495678; }

		//STATIC

		static int32 Floor(const float32 A) { return static_cast<int32>(A - (static_cast<int32>(A) % 1)); }

		static float32 Modulo(const float32 A, const float32 B)
		{
			const float32 C = A / B;
			return (C - static_cast<int32>(C)) * B;
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

		static uint64 PowerOf2RoundUp(const uint64 number, const uint32 alignment) { return (number + alignment - 1) & -static_cast<int64>(alignment); }

		static void RoundDown(const uint64 x, const uint64 multiple, uint64& quotient, uint64& remainder) { const uint64 rem = x % multiple; remainder = rem; quotient = x - rem; }
		
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
		 * \param Degrees Angle in degrees.
		 * \return Sine of Degrees
		 */
		static float32 Sine(float32 Degrees);

		/**
		 * \brief Returns the sine of an angle.
		 * \param Degrees Angle in degrees.
		 * \return Sine of Degrees
		 */
		static float64 Sine(float64 Degrees);

		/**
		* \brief Returns the cosine of an angle.
		* \param Degrees Angle in degrees.
		* \return Cosine of Degrees
		*/
		static float32 Cosine(float32 Degrees);

		/**
		* \brief Returns the cosine of an angle.
		* \param Degrees Angle in degrees.
		* \return Cosine of Degrees
		*/
		static float64 Cosine(float64 Degrees);

		/**
		* \brief Returns the tangent of an angle.
		* \param Degrees Angle in degrees.
		* \return Tangent of Degrees
		*/
		static float32 Tangent(float32 Degrees);

		/**
		* \brief Returns the tangent of an angle.
		* \param Degrees Angle in degrees.
		* \return Tangent of Degrees
		*/
		static float64 Tangent(float64 Degrees);

		/**
		* \brief Returns the arcsine of A in degrees.
		* \param A
		* \return Degrees of A
		*/
		static float32 ArcSine(float32 A);

		/**
		* \brief Returns the arccosine of A in degrees.
		* \param A
		* \return Degrees of A
		*/
		static float32 ArcCosine(float32 A);

		/**
		* \brief Returns the arctangent of A in degrees.
		* \param A
		* \return Degrees of A
		*/
		static float32 ArcTangent(float32 A);

		/**
		* \brief Returns the arctangent of Y / X in degrees.
		* \return Degrees of Y / X
		*/
		static float32 ArcTan2(float32 X, float32 Y);

		//////////////////////////////////////////////////////////////
		//						SCALAR MATH							//
		//////////////////////////////////////////////////////////////

		//Returns 1 if A is bigger than 0. 0 if A is equal to 0. and -1 if A is less than 0.
		static int8 Sign(const int64 A)
		{
			if (A > 0) { return 1; } if (A < 0) { return -1; } return 0;
		}

		//Returns 1 if A is bigger than 0. 0 if A is equal to 0. and -1 if A is less than 0.
		static int8 Sign(const float32 A)
		{
			if (A > 0.0)
			{
				return 1;
			}
			if (A < 0.0)
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

		//Interpolates from Current to Target, returns Current + an amount determined by the InterpSpeed.
		static float32 FInterp(const float32 Target, const float32 Current, const float32 DT, const float32 InterpSpeed)
		{
			return (Target - Current) * DT * InterpSpeed + Current;
		}

		static float32 MapToRange(const float32 A, const float32 InMin, const float32 InMax, const float32 OutMin, const float32 OutMax)
		{
			return InMin + ((OutMax - OutMin) / (InMax - InMin)) * (A - InMin);
		}

		static float32 obMapToRange(const float32 a, const float32 inMax, const float32 outMax) { return a / (inMax / outMax); }

		static float32 SquareRoot(const float32 a)
		{
			constexpr auto error = 0.00001; //define the precision of your result
			float64 s = a;

			while (s - a / s > error) //loop until precision satisfied 
			{
				s = (s + a / s) / 2.0;
			}

			return static_cast<float32>(s);
		}

		static float64 SquareRoot(const float64 a)
		{
			constexpr auto error = 0.00001; //define the precision of your result
			float64 s = a;

			while (s - a / s > error) //loop until precision satisfied 
			{
				s = (s + a / s) / 2.0;
			}

			return s;
		}

		static float32 Root(const float32 a, const float32 root) { return Power(a, 1.0f / root); }

		static uint32 Abs(const int32 A) { return ((A >> 31) + A) ^ (A >> 31); }

		static uint64 Abs(const int64 A) { return ((A >> 63) + A) ^ (A >> 63); }

		static float32 Abs(const float32 _A) { return _A > 0.0f ? _A : -_A; }

		template <typename T>
		static T Min(const T& A, const T& B)
		{
			return (A < B) ? A : B;
		}

		template <typename T>
		static T Max(const T& A, const T& B)
		{
			return (A > B) ? A : B;
		}

		/**
		 * \brief Returns degrees converted to radians.
		 * \param Degrees degrees to convert
		 * \return Degrees as radians
		 */
		static float32 DegreesToRadians(const float32 Degrees) { return Degrees * static_cast<float32>(PI / 180.0); }

		/**
		 * \brief Returns degrees converted to radians.
		 * \param Degrees degrees to convert
		 * \return Degrees as radians
		 */
		static float64 DegreesToRadians(const float64 Degrees) { return Degrees * (PI / 180.0); }

		/**
		 * \brief Returns Radians converted to degrees.
		 * \param Radians radians to convert.
		 * \return Radians as degrees.
		 */
		static float32 RadiansToDegrees(const float32 Radians) { return Radians * static_cast<float32>(180.0 / PI); }

		/**
		 * \brief Returns Radians converted to degrees.
		 * \param Radians radians to convert.
		 * \return Radians as degrees.
		 */
		static float64 RadiansToDegrees(const float64 Radians) { return Radians * (180.0 / PI); }

		//////////////////////////////////////////////////////////////
		//						VECTOR MATH							//
		//////////////////////////////////////////////////////////////

		//Calculates the length of a 2D vector.
		static float32 Length(const Vector2& _A)
		{
			return SquareRoot(LengthSquared(_A));
		}

		static float32 Length(const Vector2& _A, const Vector2& _B)
		{
			return SquareRoot(LengthSquared(_A - _B));
		}

		static float32 Length(const Vector3& _A)
		{
			return SquareRoot(LengthSquared(_A));
		}

		static float32 Length(const Vector3& _A, const Vector3& _B)
		{
			return SquareRoot(LengthSquared(_A - _B));
		}

		static float32 Length(const Vector4& _A)
		{
			return SquareRoot(LengthSquared(_A));
		}

		static float32 Length(const Vector4& _A, const Vector4& _B)
		{
			return SquareRoot(LengthSquared(_A - _B));
		}

		static float32 LengthSquared(const Vector2& a);

		static float32 LengthSquared(const Vector3& a);

		static float32 LengthSquared(const Vector4& a);

		static Vector2 Normalized(const Vector2& a);

		static void Normalize(Vector2& a);

		static Vector3 Normalized(const Vector3& _A);

		static void Normalize(Vector3& a);

		static Vector4 Normalized(const Vector4& a);

		static void Normalize(Vector4& a);

		static float32 DotProduct(const Vector2& _A, const Vector2& _B);

		static float32 DotProduct(const Vector3& _A, const Vector3& _B);

		static float32 DotProduct(const Vector4& _A, const Vector4& _B);

		static Vector3 Cross(const Vector3& a, const Vector3& b);

		static Vector2 Abs(const Vector2& Vec1)
		{
			return Vector2(Abs(Vec1.X), Abs(Vec1.Y));
		}

		static Vector3 Abs(const Vector3& Vec1)
		{
			return Vector3(Abs(Vec1.X), Abs(Vec1.Y), Abs(Vec1.Z));
		}

		static Vector4 Abs(const Vector4& _A)
		{
			return Vector4(Abs(_A.X), Abs(_A.Y), Abs(_A.Z), Abs(_A.Z));
		}

		static Vector2 Negated(const Vector2& Vec)
		{
			Vector2 Result;

			Result.X = -Vec.X;
			Result.Y = -Vec.Y;

			return Result;
		}

		static void Negate(Vector2& Vec)
		{
			Vec.X = -Vec.X;
			Vec.Y = -Vec.Y;

			return;
		}

		static Vector3 Negated(const Vector3& Vec)
		{
			Vector3 Result;

			Result.X = -Vec.X;
			Result.Y = -Vec.Y;
			Result.Z = -Vec.Z;

			return Result;
		}

		static void Negate(Vector3& Vec)
		{
			Vec.X = -Vec.X;
			Vec.Y = -Vec.Y;
			Vec.Z = -Vec.Z;

			return;
		}

		static Vector4 Negated(const Vector4& Vec)
		{
			Vector4 Result;

			Result.X = -Vec.X;
			Result.Y = -Vec.Y;
			Result.Z = -Vec.Z;
			Result.W = -Vec.W;

			return Result;
		}

		static void Negate(Vector4& a)
		{
			a.X = -a.X;
			a.Y = -a.Y;
			a.Z = -a.Z;
			a.W = -a.W;
		}

		//////////////////////////////////////////////////////////////
		//						QUATERNION MATH						//
		//////////////////////////////////////////////////////////////

		static float32 DotProduct(const Quaternion& a, const Quaternion& b);

		static float32 LengthSquared(const Quaternion& a);

		static float32 Length(const Quaternion& a) { return SquareRoot(LengthSquared(a)); }

		static Quaternion Normalized(const Quaternion& a);

		static void Normalize(Quaternion& a);

		static Quaternion Conjugated(const Quaternion& a) { return Quaternion(-a.X, -a.Y, -a.Z, a.Q); }

		static void Conjugate(Quaternion& a) { a.X = -a.X; a.Y = -a.Y; a.Z = -a.Z; }


		//////////////////////////////////////////////////////////////
		//						LOGIC								//
		//////////////////////////////////////////////////////////////

		static bool IsNearlyEqual(const float32 A, const float32 Target, const float32 Tolerance)
		{
			return (A > Target - Tolerance) && (A < Target + Tolerance);
		}

		static bool IsInRange(const float32 A, const float32 Min, const float32 Max)
		{
			return (A > Min) && (A < Max);
		}

		static bool IsVectorNearlyEqual(const Vector2& A, const Vector2& Target, const float32 Tolerance)
		{
			return IsNearlyEqual(A.X, Target.X, Tolerance) && IsNearlyEqual(A.Y, Target.Y, Tolerance);
		}

		static bool IsVectorNearlyEqual(const Vector3& A, const Vector3& Target, const float32 Tolerance)
		{
			if (IsNearlyEqual(A.X, Target.X, Tolerance))
			{
				if (IsNearlyEqual(A.Y, Target.Y, Tolerance))
				{
					if (IsNearlyEqual(A.Z, Target.Z, Tolerance))
					{
						return true;
					}
				}
			}

			return false;
		}

		static bool AreVectorComponentsGreater(const Vector3& A, const Vector3& B)
		{
			return A.X > B.X && A.Y > B.Y && A.Z > B.Z;
		}

		//////////////////////////////////////////////////////////////
		//						MATRIX MATH							//
		//////////////////////////////////////////////////////////////

		//Creates a translation matrix.
		static Matrix4 Translation(const Vector3& Vector)
		{
			Matrix4 result(1);

			result(0, 3) = Vector.X;
			result(1, 3) = Vector.Y;
			result(2, 3) = Vector.Z;

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
			Vector3 tangent1 = Normalized(Cross(normal, tangent0));
			return Matrix4(tangent0.X, tangent0.Y, tangent0.Z, 0.0f, tangent1.X, tangent1.Y, tangent1.Z, 0.0f, normal.X, normal.Y, normal.Z, 0.0f, 0, 0, 0, 0);
		}

		static void Rotate(Matrix4& A, const Quaternion& Q)
		{
			const auto rotation = Rotation(Q);

			A *= rotation;
		}

		static Vector3 SphericalCoordinatesToCartesianCoordinates(const Vector2& sphericalCoordinates)
		{
			auto cy = Cosine(sphericalCoordinates.Y);

			return Vector3(cy * Sine(sphericalCoordinates.X), Sine(sphericalCoordinates.Y),	cy * Cosine(sphericalCoordinates.X));
		}

		static Vector3 RotatorToNormalVector(const Rotator& rotator)
		{
			auto x = Cosine(rotator.Y) * Cosine(rotator.X);
			auto y = Sine(rotator.Y) * Cosine(rotator.X);
			auto z = Sine(rotator.X);

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
			result.X = sy * cp * sr + cy * sp * cr;
			result.Y = sy * cp * cr - cy * sp * sr;
			result.Z = cy * cp * sr - sy * sp * cr;
			result.Q = cy * cp * cr + sy * sp * sr;

			return result;
		}

		static Matrix4 Rotation(const Quaternion& A)
		{
			Matrix4 result(1);

			auto xx = A.X * A.X;
			auto xy = A.X * A.Y;
			auto xz = A.X * A.Z;
			auto xw = A.X * A.Q;
			auto yy = A.Y * A.Y;
			auto yz = A.Y * A.Z;
			auto yw = A.Y * A.Q;
			auto zz = A.Z * A.Z;
			auto zw = A.Z * A.Q;

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

			float32 c = Cosine(angle); // cosine
			float32 s = Sine(angle); // sine
			float32 xx = A.X * A.X;
			float32 xy = A.X * A.Y;
			float32 xz = A.X * A.Z;
			float32 yy = A.Y * A.Y;
			float32 yz = A.Y * A.Z;
			float32 zz = A.Z * A.Z;

			// build rotation matrix
			Matrix4 m;
			m[0] = xx * (1 - c) + c;
			m[1] = xy * (1 - c) - A.Z * s;
			m[2] = xz * (1 - c) + A.Y * s;
			m[3] = 0;
			m[4] = xy * (1 - c) + A.Z * s;
			m[5] = yy * (1 - c) + c;
			m[6] = yz * (1 - c) - A.X * s;
			m[7] = 0;
			m[8] = xz * (1 - c) - A.Y * s;
			m[9] = yz * (1 - c) + A.X * s;
			m[10] = zz * (1 - c) + c;
			m[11] = 0;
			m[12] = 0;
			m[13] = 0;
			m[14] = 0;
			m[15] = 1;

			return result;
		}

		static void Scale(Matrix4& A, const Vector3& B)
		{
			const auto scaling = Scaling(B);

			A *= scaling;
		}

		static Matrix4 Scaling(const Vector3& A)
		{
			Matrix4 Result;

			Result[0] = A.X;
			Result[5] = A.Y;
			Result[10] = A.Z;

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

		static void BuildPerspectiveMatrix(Matrix4& matrix, float32 fov, float32 aspectRatio, float32 near, float32 far)
		{
			//Tangent of half the vertical view angle.
			const auto f = 1 / Math::Tangent(fov * 0.5f);

			const auto far_m_near = far - near;

			//Zero to one
			//Left handed

			matrix(0, 0) = f / aspectRatio;

			matrix(1, 1) = -f;

			matrix(2, 2) = -((far + near) / far_m_near);
			matrix(2, 3) = -((2.f * far * near) / far_m_near);

			matrix(3, 2) = -1.0f;
		}

		static void MakeOrthoMatrix(Matrix4& matrix, float32 right, float32 left, float32 top, float32 bottom, float32 near, float32 far)
		{
			//Zero to one
			//Left handed

			matrix(0, 0) = static_cast<float32>(2) / (right - left);
			matrix(1, 1) = static_cast<float32>(2) / (top - bottom);
			matrix(2, 2) = static_cast<float32>(1) / (far - near);
			matrix(3, 0) = -(right + left) / (right - left);
			matrix(3, 1) = -(top + bottom) / (top - bottom);
			matrix(3, 2) = -near / (far - near);
		}

		static float32 Clamp(float32 _A, float32 _Min, float32 _Max)
		{
			return _A > _Max ? _Max : _A < _Min ? _Min : _A;
		}

		static Vector3 ClosestPointOnPlane(const Vector3& _Point, const Plane& _Plane)
		{
			const float32 T = (DotProduct(_Plane.Normal, _Point) - _Plane.D) / DotProduct(_Plane.Normal, _Plane.Normal);
			return _Point - _Plane.Normal * T;
		}

		static float64 DistanceFromPointToPlane(const Vector3& _Point, const Plane& _Plane)
		{
			// return Dot(q, p.n) - p.d; if plane equation normalized (||p.n||==1)
			return (DotProduct(_Plane.Normal, _Point) - _Plane.D) / DotProduct(_Plane.Normal, _Plane.Normal);
		}

		static void ClosestPointOnLineSegmentToPoint(const Vector3& _C, const Vector3& _A, const Vector3& _B,
			float32& _T, Vector3& _D)
		{
			const Vector3 AB = _B - _A;
			// Project c onto ab, computing parameterized position d(t) = a + t*(b – a)
			_T = DotProduct(_C - _A, AB) / DotProduct(AB, AB);
			// If outside segment, clamp t (and therefore d) to the closest endpoint
			if (_T < 0.0) _T = 0.0;
			if (_T > 1.0) _T = 1.0;
			// Compute projected position from the clamped t
			_D = _A + AB * _T;
		}

		static float64 SquaredDistancePointToSegment(const Vector3& _A, const Vector3& _B, const Vector3& _C)
		{
			const Vector3 AB = _B - _A;
			const Vector3 AC = _C - _A;
			const Vector3 BC = _C - _B;
			float32 E = DotProduct(AC, AB);
			// Handle cases where c projects outside ab
			if (E <= 0.0f) return DotProduct(AC, AC);
			float32 f = DotProduct(AB, AB);
			if (E >= f) return DotProduct(BC, BC);
			// Handle cases where c projects onto ab
			return DotProduct(AC, AC) - E * E / f;
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
			float32 VA = D3 * D6 - D5 * D4;
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

		template<typename T1, typename T2, typename MF, typename SF>
		static void MultiplesFor(const Ranger<T1>& range1, const Ranger<T2>& range2, const uint64 multiple, const MF& multiplesFunction, const SF& singlesFunction)
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
}
