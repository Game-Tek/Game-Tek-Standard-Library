#include <gtest/gtest.h>

#include "GTSL/Math/Collision.hpp"

struct SupportSphere {

	SupportSphere(const GTSL::Vector3 pos, const GTSL::float32 r) : position(pos), radius(r) {}

	auto GetPosition() const { return position; }

	auto GetSupportPointInDirection(const GTSL::Vector3 dir) {
		return position + dir * radius;
	}

	GTSL::Vector3 position;
	GTSL::float32 radius;
};

TEST(GJK, NoIntersection) {
	SupportSphere a({ 0, 0, 0 }, 1), b({ 0, 2.5, 0 }, 1);
	EXPECT_FALSE(GTSL::GJK(a, b));
}

TEST(GJK, Intersection) {
	SupportSphere a({ 0.5f, -0.5, 0.1f }, 1), b({ 0.3f, 0.0f, 0.2f }, 1);
	EXPECT_TRUE(GTSL::GJK(a, b));
}