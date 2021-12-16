#include <gtest/gtest.h>

#include "GTSL/HashMap.hpp"
#include "GTSL/Vector.hpp"

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

TEST(HashMap, FindAndResize) {
	GTSL::HashMap<GTSL::uint64, GTSL::uint64, GTSL::DefaultAllocatorReference> hashMap(2);

	for (GTSL::uint32 i = 0; i < 8193; ++i) {
		hashMap.Emplace(i, i);
	}

	for (GTSL::uint32 i = 0; i < 8193; ++i) {
		ASSERT_TRUE(hashMap.Find(i));
	}
}

TEST(HashMap, Iterator) {
	{
		GTSL::HashMap<GTSL::uint64, GTSL::uint64, GTSL::DefaultAllocatorReference> hashMap(2);

		GTSL::uint32 sum = 0;
		constexpr GTSL::uint32 COUNT = 1025;

		for (GTSL::uint32 i = 0; i < COUNT; ++i) {
			hashMap.Emplace(i, i);
			sum += i;
		}

		GTSL::uint32 i = 0, res = 0;
		for (const auto& e : hashMap) {
			++i;
			res += e;
		}

		GTEST_ASSERT_EQ(i, COUNT);
		GTEST_ASSERT_EQ(sum, res);
	}

	{
		GTSL::HashMap<GTSL::uint64, GTSL::uint64, GTSL::DefaultAllocatorReference> hashMap;
		for (const auto& e : hashMap) {
			std::cout << e;
		}
	}
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

TEST(KeyMap, Construct) {
	GTSL::KeyMap<GTSL::uint64, GTSL::DefaultAllocatorReference> keyMap(4);
}

TEST(KeyMap, Emplace) {
	GTSL::KeyMap<GTSL::uint64, GTSL::DefaultAllocatorReference> keyMap(4);

	keyMap.Emplace(123);
}

TEST(KeyMap, FindAndResize) {
	GTSL::KeyMap<GTSL::uint64, GTSL::DefaultAllocatorReference> keyMap(2);

	for(GTSL::uint32 i = 0; i < 1025; ++i) {
		keyMap.Emplace(i);
	}

	for (GTSL::uint32 i = 0; i < 1025; ++i) {
		ASSERT_TRUE(keyMap.Find(i));
	}
}