#include <gtest/gtest.h>

#include "GTSL/FixedVector.hpp"

TEST(FixedVector, Construct) {
	GTSL::FixedVector<GTSL::uint32, GTSL::DefaultAllocatorReference> fixedVector;
}

TEST(FixedVector, Resize) {
	GTSL::FixedVector<GTSL::uint32, GTSL::DefaultAllocatorReference> fixedVector(1);

	EXPECT_EQ(fixedVector.GetCapacity(), 32u); // Capacity must be multiple of BITS=32

	GTSL::uint32 toInsert = fixedVector.GetCapacity() + 10000;

	for(GTSL::uint32 i = 0; i < toInsert; ++i) {
		fixedVector.Emplace(i);
	}

	for(GTSL::uint32 i = 0; i < toInsert; ++i) {
		ASSERT_TRUE(fixedVector.IsSlotOccupied(i));
		GTEST_ASSERT_EQ(fixedVector[i], i);
	}

	for (GTSL::uint32 i = toInsert; i < fixedVector.GetCapacity(); ++i) {
		ASSERT_FALSE(fixedVector.IsSlotOccupied(i));
	}
}
	