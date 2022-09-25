/* #include "GTSL/IndirectionMap.hpp"

#include <gtest/gtest.h>

#include "GTSL/Allocator.h"
#include "GTSL/Vector.hpp"

TEST(IndirectionMap, Insertion) {
	GTSL::IndirectionMap<GTSL::DefaultAllocatorReference> map;

	map.Insert(6);
	map.Insert(2);
	map.Insert(4);
	map.Insert(3);

	GTEST_ASSERT_EQ(map[2], 1);
	GTEST_ASSERT_EQ(map[3], 2);
	GTEST_ASSERT_EQ(map[4], 3);
	GTEST_ASSERT_EQ(map[6], 0);
}

TEST(IndirectionMap, RandomInsertion) {
	GTSL::IndirectionMap<GTSL::DefaultAllocatorReference> map;

	GTSL::StaticVector<GTSL::uint32, 2048> keyVec;

	GTSL::Math::RandomSeed randomKey(-2323469737);

	for(GTSL::uint32 i = 0; i < 1024; ++i) {
		map.Insert(keyVec.EmplaceBack(randomKey()));
	}

	for(GTSL::uint32 i = 0; i < 1024; ++i) {
		GTEST_ASSERT_EQ(map[keyVec[i]], i);
	}
}

TEST(IndirectionMap, Removal) {
	GTSL::IndirectionMap<GTSL::DefaultAllocatorReference> map;

	map.Insert(6);
	map.Insert(2);
	map.Insert(4);
	map.Insert(3);

	map.Remove(2);

	map.Insert(2);

	GTEST_ASSERT_EQ(map[2], 1);
	GTEST_ASSERT_EQ(map[3], 2);
	GTEST_ASSERT_EQ(map[4], 3);
	GTEST_ASSERT_EQ(map[6], 0);
} */