//module;

#pragma once

#include "Math.hpp"
#include "Vectors.hpp"
#include <array>

#include "GTSL/Pair.hpp"
#include "GTSL/Vector.hpp"

//export module Collision;

namespace GTSL {
	class Simplex {
	public:
		void AddPoint(GTSL::Vector3 newPoint) {
			points[length++] = newPoint;
		}

		void Remove(const uint8 index) {
			//for (uint8 i = index; i < length - 1; ++i) {
			//	points[i] = points[i + 1];
			//}

			--length;
		}

		uint8 GetLength() const { return length; }

		GTSL::Vector3 operator[](const uint8 index) const { return points[index]; }

	private:
		GTSL::Vector3 points[4]; uint8 length = 0;
	};

	inline bool GJK(auto& objectA, auto& objectB) {
		GTSL::Vector3 direction = GTSL::Math::Normalized(objectB.GetPosition() - objectA.GetPosition());

		auto supportPoint = objectA.GetSupportPointInDirection(direction) - objectB.GetSupportPointInDirection(-direction);

		Simplex simplex;

		simplex.AddPoint(supportPoint);

		direction = -supportPoint;

		while (true) {
			supportPoint = objectA.GetSupportPointInDirection(direction) - objectB.GetSupportPointInDirection(-direction);

			if (GTSL::Math::DotProduct(supportPoint, direction) <= 0) //if new point doesn't goes past the origin
				return false;

			simplex.AddPoint(supportPoint);

			switch (simplex.GetLength()) {
			case 2: { //line
				auto ab = simplex[1] - simplex[0];
				auto a0 = -simplex[0];

				if (GTSL::Math::DotProduct(ab, a0) > 0.0f) { //
					direction = ((simplex[0] + simplex[1]) * -0.5f);
				}
				else {
					simplex.Remove(1);
					direction = a0;
				}

				break;
			}
			case 3: { //triangle
				auto a = simplex[0], b = simplex[1], c = simplex[2];
				auto ab = b - a;
				auto ac = c - a;
				auto a0 = -a;

				auto abPerp = GTSL::Math::TripleProduct(ac, ab);
				auto acPerp = GTSL::Math::TripleProduct(ab, ac);

				if (GTSL::Math::DotProduct(abPerp, a0) > 0) {
					simplex.Remove(2);
					direction = abPerp;
				}
				else if (GTSL::Math::DotProduct(acPerp, a0) > 0) {
					simplex.Remove(1);
					direction = acPerp;
				}

				break;
			}
			case 4: { //tetra
				auto a = simplex[0], b = simplex[1], c = simplex[2], d = simplex[3];
				auto ab = b - a, ac = c - a, ad = d - a, a0 = -a;

				auto abc = GTSL::Math::Cross(ab, ac);
				auto acd = GTSL::Math::Cross(ac, ad);
				auto adb = GTSL::Math::Cross(ad, ab);

				if (GTSL::Math::DotProduct(abc, a0) > 0.0f) {
					// the origin is not here, remove d
					simplex.Remove(3);
					direction = abc;
				}
				else if (GTSL::Math::DotProduct(acd, a0) > 0.0f) {
					simplex.Remove(1);
					direction = acd;
				}
				else if (GTSL::Math::DotProduct(adb, a0) > 0.0f) {
					simplex.Remove(2);
					direction = adb;
				}
				else {
					return true;
				}

				break;
			}
			}
		}
	}

	struct CollisionInfo {
		GTSL::Vector3 A, B, Normal;
		float32 Depth;
	};

	inline void GetFaceNormals(GTSL::Range<const GTSL::Vector3*> polytope, GTSL::Range<const std::array<uint16, 3>*> indices, auto& normals, uint32& minFace) {
		float32 minDistance = FLT_MAX;

		for (uint32 f = 0; f < indices.ElementCount(); ++f) {
			auto a = polytope[indices[f][0]], b = polytope[indices[f][1]], c = polytope[indices[f][2]];

			auto normal = GTSL::Math::Normalized(GTSL::Math::Cross(b - a, c - a));
			auto distance = GTSL::Math::DotProduct(normal, a);

			if (distance < 0.0f) { //normal check, flip if wrong sided
				normal *= -1.0f; distance *= -1.0f;
			}

			normals.EmplaceBack(normal, distance);

			if (distance < minDistance) {
				minFace = f;
				minDistance = distance;
			}
		}
	}

	template<class ALLOCATOR>
	CollisionInfo EPA(auto& objectA, auto& objectB, const Simplex& simplex, const ALLOCATOR& allocator) {
		GTSL::SemiVector<GTSL::Vector3, 64, ALLOCATOR> polytope(4, allocator);
		polytope.EmplaceBack(simplex[0]); polytope.EmplaceBack(simplex[1]); polytope.EmplaceBack(simplex[2]); polytope.EmplaceBack(simplex[3]);
		GTSL::SemiVector<std::array<GTSL::uint16, 3>, 64, ALLOCATOR> indices(4, allocator);
		indices.EmplaceBack({ 0, 1, 2 }); indices.EmplaceBack({ 0, 3, 1 }); indices.EmplaceBack({ 0, 2, 3 }); indices.EmplaceBack({ 1, 3, 2 });

		SemiVector<Pair<GTSL::Vector3, float32>, 64, ALLOCATOR> normals(4, allocator); uint32 minFace = 0;
		GetFaceNormals(polytope, indices, normals, minFace);

		constexpr float32 FLOAT_MAX = 3.402823466e+38F;

		GTSL::Vector3 minNormal; float32 minDistance = FLOAT_MAX;

		while (minDistance == FLOAT_MAX) {
			minNormal = GTSL::Vector3(normals[minFace].First);
			minDistance = normals[minFace].Second;

			auto supportPoint = objectA.GetSupportPointInDirection(minNormal) - objectB.GetSupportPointInDirection(-minNormal);
			float32 sDistance = GTSL::Math::DotProduct(minNormal, supportPoint);

			if (GTSL::Math::Abs(sDistance - minDistance) > 0.001f) {
				minDistance = FLOAT_MAX;

				GTSL::SemiVector<GTSL::Pair<uint16, uint16>, 64, ALLOCATOR> uniqueEdges(4, allocator);

				for (uint32 i = 0; i < normals.GetLength(); ++i) {
					if (GTSL::Math::DotProduct(normals[i].First, supportPoint) > 0.0f) {
						auto addIfUniqueEdge = [&](uint32 fIndex, uint32 f0, uint32 f1) {

							if (const auto searchResult = uniqueEdges.Find({ indices[fIndex][f1], indices[fIndex][f0] }); searchResult) {
								uniqueEdges.Pop(searchResult.Get());
							}
							else {
								uniqueEdges.EmplaceBack(indices[fIndex][f0], indices[fIndex][f1]);
							}
						};

						addIfUniqueEdge(i, 0, 1);
						addIfUniqueEdge(i, 1, 2);
						addIfUniqueEdge(i, 2, 0);

						indices[i][2] = indices.back()[2]; indices[i][1] = indices.back()[1]; indices[i][0] = indices.back()[0];
						indices.PopBack();

						normals[i] = normals.back();
						normals.PopBack();

						--i;
					}
				}

				GTSL::SemiVector<std::array<uint16, 3>, 64, ALLOCATOR> newFaces(4, allocator);

				for (auto& e : uniqueEdges) {
					newFaces.EmplaceBack(std::array{ e.First, e.Second, static_cast<uint16>(polytope.GetLength()) });
				}

				polytope.EmplaceBack(supportPoint);

				GTSL::SemiVector<GTSL::Pair<GTSL::Vector3, float32>, 64, ALLOCATOR> newNormals(4, allocator); uint32 newMinFace = 0;
				GetFaceNormals(polytope, indices, newNormals, newMinFace);

				float32 oldMinDistance = FLOAT_MAX;
				for (uint32 i = 0; i < normals.GetLength(); ++i) {
					if (normals[i].Second < oldMinDistance) {
						oldMinDistance = normals[i].Second;
						minFace = i;
					}
				}

				if (newNormals[newMinFace].Second < oldMinDistance) {
					minFace = newMinFace + normals.GetLength();
				}

				indices.PushBack(newFaces); normals.PushBack(newNormals);
			}
		}

		CollisionInfo collision_info;
		collision_info.Normal = minNormal;
		collision_info.Depth = minDistance + 0.0001f;

		return collision_info;
	}
}