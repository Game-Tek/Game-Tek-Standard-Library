#include <gtest/gtest.h>

#include "GTSL/Vector.hpp"
#include <GTSL/Tuple.hpp>

#include "GTSL/Delegate.hpp"

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

	Call(lambda, GTSL::Tuple(0u, 1u));
}

TEST(Delegate, Valid) {
	auto lambda = [](GTSL::uint32 a, GTSL::uint32 b) {
		GTEST_ASSERT_EQ(a, 0);
		GTEST_ASSERT_EQ(b, 1);
	};

	auto delegate = GTSL::Delegate<void(GTSL::uint32, GTSL::uint32)>::Create(lambda);

	delegate(0u, 1u);

	Call(GTSL::Delegate<void(GTSL::uint32, GTSL::uint32)>::Create(lambda), GTSL::Tuple(0u, 1u));
}

using TTT = GTSL::Tuple<GTSL::float32, GTSL::uint32, GTSL::uint8, GTSL::uint16, GTSL::uint64>;
static_assert(GTSL::GetTypeIndexInTuple<GTSL::float32>(TTT()) == 0);
static_assert(GTSL::GetTypeIndexInTuple<GTSL::uint16>(TTT()) == 3);

static_assert(GTSL::IsInTuple<GTSL::float32>(TTT()));
static_assert(!GTSL::IsInTuple<GTSL::int16>(TTT()));

static_assert(GTSL::IsSame<GTSL::TupleElement<0, TTT>::type, GTSL::float32>());

static_assert(GTSL::GetTypeIndex<GTSL::float32, GTSL::float32, GTSL::uint32, GTSL::uint8, GTSL::uint16, GTSL::uint64>() == 0);
static_assert(GTSL::GetTypeIndex<GTSL::uint64, GTSL::float32, GTSL::uint32, GTSL::uint8, GTSL::uint16, GTSL::uint64>() == 4);

static_assert(GTSL::IsSame<GTSL::TypeAt<0, GTSL::uint64, GTSL::float32, GTSL::uint32, GTSL::uint8, GTSL::uint16, GTSL::uint64>::type, GTSL::uint64>());
static_assert(GTSL::IsSame<GTSL::TypeAt<1, GTSL::uint64, GTSL::float32, GTSL::uint32, GTSL::uint8, GTSL::uint16, GTSL::uint64>::type, GTSL::float32>());
static_assert(GTSL::IsSame<GTSL::TypeAt<3, GTSL::uint64, GTSL::float32, GTSL::uint32, GTSL::uint8, GTSL::uint16, GTSL::uint64>::type, GTSL::uint8>());