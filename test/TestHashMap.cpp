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

TEST(HashMap, Pop) {
	GTSL::HashMap<GTSL::uint64, GTSL::uint64, GTSL::DefaultAllocatorReference> hashMap(2);

	hashMap.Emplace(2, 1); hashMap.Emplace(3, 2);
	hashMap.Emplace(0, 3); hashMap.Emplace(1, 4);

	hashMap.Remove(0);

	{
		auto ref = hashMap.TryGet(0);
		GTEST_ASSERT_EQ(ref.State(), false);
	}

	hashMap.Remove(2);

	{
		auto ref = hashMap.TryGet(2);
		GTEST_ASSERT_EQ(ref.State(), false);
	}

	{
		auto ref = hashMap.TryGet(1);
		GTEST_ASSERT_EQ(ref.State(), true); GTEST_ASSERT_EQ(ref.Get(), 4);
	}

	{
		auto ref = hashMap.TryGet(3);
		GTEST_ASSERT_EQ(ref.State(), true); GTEST_ASSERT_EQ(ref.Get(), 2);
	}
}

TEST(HashMap, Resize) {
	GTSL::HashMap<GTSL::uint64, GTSL::uint64, GTSL::DefaultAllocatorReference> hashMap(2);

	hashMap.Emplace(2, 1); hashMap.Emplace(3, 2);
	hashMap.Emplace(0, 3); hashMap.Emplace(1, 4);

	{
		auto ref = hashMap.TryGet(0);
		GTEST_ASSERT_EQ(ref.State(), true); GTEST_ASSERT_EQ(ref.Get(), 3);
	}

	{
		auto ref = hashMap.TryGet(1);
		GTEST_ASSERT_EQ(ref.State(), true); GTEST_ASSERT_EQ(ref.Get(), 4);
	}

	{
		auto ref = hashMap.TryGet(2);
		GTEST_ASSERT_EQ(ref.State(), true); GTEST_ASSERT_EQ(ref.Get(), 1);
	}

	{
		auto ref = hashMap.TryGet(3);
		GTEST_ASSERT_EQ(ref.State(), true); GTEST_ASSERT_EQ(ref.Get(), 2);
	}

	hashMap.Emplace(7, 16); hashMap.Emplace(11, 13);
	hashMap.Emplace(15, 999); hashMap.Emplace(19, 111);

	{
		auto ref = hashMap.TryGet(0);
		GTEST_ASSERT_EQ(ref.State(), true); GTEST_ASSERT_EQ(ref.Get(), 3);
	}

	{
		auto ref = hashMap.TryGet(1);
		GTEST_ASSERT_EQ(ref.State(), true); GTEST_ASSERT_EQ(ref.Get(), 4);
	}

	{
		auto ref = hashMap.TryGet(2);
		GTEST_ASSERT_EQ(ref.State(), true); GTEST_ASSERT_EQ(ref.Get(), 1);
	}

	{
		auto ref = hashMap.TryGet(3);
		GTEST_ASSERT_EQ(ref.State(), true); GTEST_ASSERT_EQ(ref.Get(), 2);
	}

	{
		auto ref = hashMap.TryGet(7);
		GTEST_ASSERT_EQ(ref.State(), true); GTEST_ASSERT_EQ(ref.Get(), 16);
	}

	{
		auto ref = hashMap.TryGet(11);
		GTEST_ASSERT_EQ(ref.State(), true); GTEST_ASSERT_EQ(ref.Get(), 13);
	}

	{
		auto ref = hashMap.TryGet(15);
		GTEST_ASSERT_EQ(ref.State(), true); GTEST_ASSERT_EQ(ref.Get(), 999);
	}

	{
		auto ref = hashMap.TryGet(19);
		GTEST_ASSERT_EQ(ref.State(), true); GTEST_ASSERT_EQ(ref.Get(), 111);
	}
}

TEST(KeyMap, Construct) {
	GTSL::KeyMap<GTSL::uint64, GTSL::DefaultAllocatorReference> keyMap(4);
}

TEST(KeyMap, Emplace) {
	GTSL::KeyMap<GTSL::uint64, GTSL::DefaultAllocatorReference> keyMap(4);

	keyMap.Emplace(123);
}

TEST(KeyMap, Find) {
	GTSL::KeyMap<GTSL::uint64, GTSL::DefaultAllocatorReference> keyMap(4);
	keyMap.Emplace(123);

	ASSERT_TRUE(keyMap.Find(123));

	ASSERT_FALSE(keyMap.Find(456));

	keyMap.Emplace(6789);

	ASSERT_TRUE(keyMap.Find(123));
	ASSERT_FALSE(keyMap.Find(456));
	ASSERT_TRUE(keyMap.Find(6789));
}