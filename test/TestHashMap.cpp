#include <gtest/gtest.h>

#include "GTSL/HashMap.hpp"

TEST(HashMap, Construct) {
	GTSL::HashMap<GTSL::uint64, GTSL::uint64, GTSL::DefaultAllocatorReference> hashMap(2);
}

TEST(HashMap, Emplace) {
	GTSL::HashMap<GTSL::uint64, GTSL::uint64, GTSL::DefaultAllocatorReference> hashMap(2);

	auto& ref0 = hashMap.Emplace(0, 0);
	GTEST_ASSERT_EQ(ref0, 0);

	auto& ref1 = hashMap.Emplace(1, 1);
	GTEST_ASSERT_EQ(ref1, 1);
}

TEST(HashMap, Lookup) {
	GTSL::HashMap<GTSL::uint64, GTSL::uint64, GTSL::DefaultAllocatorReference> hashMap(2);

	auto& ref0 = hashMap.Emplace(0, 23);
	GTEST_ASSERT_EQ(ref0, hashMap[0]);

	auto& ref1 = hashMap.Emplace(1, 24);
	GTEST_ASSERT_EQ(ref1, hashMap[1]);

	GTEST_ASSERT_EQ(ref0, hashMap[0]);
}

TEST(HashMap, Find) {
	GTSL::HashMap<GTSL::uint64, GTSL::uint64, GTSL::DefaultAllocatorReference> hashMap(2);

	auto ref0 = hashMap.Find(0);
	GTEST_ASSERT_EQ(ref0, false);

	hashMap.Emplace(0, 23);

	ref0 = hashMap.Find(0);
	GTEST_ASSERT_EQ(ref0, true);

	auto ref1 = hashMap.Find(1);
	GTEST_ASSERT_EQ(ref1, false);

	hashMap.Emplace(1, 23);

	ref1 = hashMap.Find(1);
	GTEST_ASSERT_EQ(ref1, true);

	ref0 = hashMap.Find(0);
	GTEST_ASSERT_EQ(ref0, true);
}