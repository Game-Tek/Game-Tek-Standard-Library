#include <gtest/gtest.h>

#include "GTSL/FixedVector.hpp"

TEST(FixedVector, Construct) {
	GTSL::FixedVector<GTSL::uint32, GTSL::DefaultAllocatorReference> fixedVector;
}

TEST(FixedVector, Resize) {
	GTSL::FixedVector<GTSL::uint32, GTSL::DefaultAllocatorReference> fixedVector(1);

	GTSL::uint32 toInsert = fixedVector.GetCapacity() + 1;

	for(GTSL::uint32 i = 0; i < toInsert + 1000; ++i) {
		fixedVector.Emplace(i);
	}

	GTSL::uint32 i = 0;

	for (auto & e : fixedVector) {
		GTEST_ASSERT_EQ(e, i);
		ASSERT_TRUE(fixedVector.IsSlotOccupied(i));
		++i;
	}

	for (; i < fixedVector.GetCapacity(); ++i) {
		ASSERT_FALSE(fixedVector.IsSlotOccupied(i));
	}
}
	