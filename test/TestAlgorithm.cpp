#include <gtest/gtest.h>

#include "GTSL/Vector.hpp"

TEST(CrossSearch, Invalid) {
	GTSL::StaticVector<GTSL::uint32, 8> a{ 1, 2, 3, 4, 5, 6, 7, 8 };
	GTSL::StaticVector<GTSL::uint32, 8> b{ 9, 10, 11, 12, 13, 14, 15 };

	GTEST_ASSERT_EQ(CrossSearch(a, b, 6u), false);
	GTEST_ASSERT_EQ(CrossSearch(a, b, 15), false);
	GTEST_ASSERT_EQ(CrossSearch(a, b, 1u), false);
	GTEST_ASSERT_EQ(CrossSearch(a, b, 9u), false);
	GTEST_ASSERT_EQ(CrossSearch(a, b, 18u), false);
}

TEST(CrossSearch, Valid) {
	GTSL::StaticVector<GTSL::uint32, 8> a{ 1, 2, 3, 4, 5, 6, 7, 8 };
	GTSL::StaticVector<GTSL::uint32, 8> b{ 7, 6, 5, 4, 3, 2, 1, 0 };

	GTEST_ASSERT_EQ(CrossSearch(a, b, 1u), true);
	GTEST_ASSERT_EQ(CrossSearch(a, b, 2u), true);
	GTEST_ASSERT_EQ(CrossSearch(a, b, 3u), true);
	GTEST_ASSERT_EQ(CrossSearch(a, b, 4u), true);
	GTEST_ASSERT_EQ(CrossSearch(a, b, 5u), true);
}

TEST(Sort, Greater) {
	GTSL::StaticVector<GTSL::uint32, 16> a{ 7, 6, 5, 4, 3, 2, 1, 0, 16, 67, 128, 111, 37 };

	SortG(a);

	GTEST_ASSERT_EQ(a[0], 0);
	GTEST_ASSERT_EQ(a[1], 1);
	GTEST_ASSERT_EQ(a[2], 2);
	GTEST_ASSERT_EQ(a[3], 3);
	GTEST_ASSERT_EQ(a[4], 4);
	GTEST_ASSERT_EQ(a[5], 5);
	GTEST_ASSERT_EQ(a[6], 6);
	GTEST_ASSERT_EQ(a[7], 7);
}

TEST(Sort, Less) {
	GTSL::StaticVector<GTSL::uint32, 16> a{ 0, 1, 2, 3, 4, 5, 6, 7 };

	SortL(a);

	GTEST_ASSERT_EQ(a[0], 7);
	GTEST_ASSERT_EQ(a[1], 6);
	GTEST_ASSERT_EQ(a[2], 5);
	GTEST_ASSERT_EQ(a[3], 4);
	GTEST_ASSERT_EQ(a[4], 3);
	GTEST_ASSERT_EQ(a[5], 2);
	GTEST_ASSERT_EQ(a[6], 1);
	GTEST_ASSERT_EQ(a[7], 0);
}

TEST(Call, Valid) {
	auto lambda = [](GTSL::uint32 a, GTSL::uint32 b) {
		GTEST_ASSERT_EQ(a, 0);
		GTEST_ASSERT_EQ(b, 1);
	};

	Call(lambda, GTSL::Tuple(0, 1));
}
