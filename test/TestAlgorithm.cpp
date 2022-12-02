#include <gtest/gtest.h>

#include "GTSL/Vector.hpp"
#include <GTSL/Tuple.hpp>

#include "GTSL/ArrayCommon.hpp"
#include "GTSL/Delegate.hpp"
#include "GTSL/Bitman.h"

TEST(AllContain, Invalid) {
	GTSL::StaticVector<GTSL::uint32, 8> a{ 1, 2, 3, 4, 5, 6, 7, 8 };
	GTSL::StaticVector<GTSL::uint32, 8> b{ 9, 10, 11, 12, 13, 14, 15 };

	GTEST_ASSERT_EQ(GTSL::Contain(GTSL::ref_tuple(a, b), 6u), false);
	GTEST_ASSERT_EQ(GTSL::Contain(GTSL::ref_tuple(a, b), 15), false);
	GTEST_ASSERT_EQ(GTSL::Contain(GTSL::ref_tuple(a, b), 1u), false);
	GTEST_ASSERT_EQ(GTSL::Contain(GTSL::ref_tuple(a, b), 9u), false);
	GTEST_ASSERT_EQ(GTSL::Contain(GTSL::ref_tuple(a, b), 18u), false);
}

TEST(AllContain, Valid) {
	GTSL::StaticVector<GTSL::uint32, 8> a{ 1, 2, 3, 4, 5, 6, 7, 8 };
	GTSL::StaticVector<GTSL::uint32, 8> b{ 7, 6, 5, 4, 3, 2, 1, 0 };

	GTEST_ASSERT_EQ(GTSL::Contain(GTSL::ref_tuple(a, b), 1u), true);
	GTEST_ASSERT_EQ(GTSL::Contain(GTSL::ref_tuple(a, b), 2u), true);
	GTEST_ASSERT_EQ(GTSL::Contain(GTSL::ref_tuple(a, b), 3u), true);
	GTEST_ASSERT_EQ(GTSL::Contain(GTSL::ref_tuple(a, b), 4u), true);
	GTEST_ASSERT_EQ(GTSL::Contain(GTSL::ref_tuple(a, b), 5u), true);
}

TEST(Contains, Array) {
	GTSL::StaticVector<GTSL::uint32, 8> a{ 3u, 6u, 9u, 2u, 4u };

	ASSERT_TRUE(Contains(a, 6u));
	ASSERT_TRUE(Contains(a, 4u));
	ASSERT_TRUE(Contains(a, 3u));

	ASSERT_FALSE(Contains(a, 0u));
}

TEST(Sort, Greater) {
	GTSL::StaticVector<GTSL::uint32, 16> a{ 7, 6, 5, 4, 3, 2, 1, 0, 16, 67, 128, 111, 37 };

	SortAscending(a);

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

	SortDescending(a);

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

TEST(ArrayFunctions, Insert) {
	GTSL::uint32 arr[10];

	GTSL::InsertElement(10, 0, arr, 0, 0);

	GTEST_ASSERT_EQ(arr[0], 0u);

	GTSL::InsertElement(10, 1, arr, 1, 1);

	GTEST_ASSERT_EQ(arr[0], 0u);
	GTEST_ASSERT_EQ(arr[1], 1u);

	GTSL::InsertElement(10, 2, arr, 1, 2);

	GTEST_ASSERT_EQ(arr[0], 0u);
	GTEST_ASSERT_EQ(arr[1], 2u);
	GTEST_ASSERT_EQ(arr[2], 1u);
}

using TTT = GTSL::Tuple<GTSL::float32, GTSL::uint32, GTSL::uint8, GTSL::uint16, GTSL::uint64>;
static_assert(GTSL::TypeIndexInTuple<GTSL::float32, TTT>::value == 0);
static_assert(GTSL::TypeIndexInTuple<GTSL::uint16, TTT>::value == 3);

static_assert(GTSL::IsInTuple<GTSL::float32>(TTT()));
static_assert(!GTSL::IsInTuple<GTSL::int16>(TTT()));

static_assert(GTSL::IsSame<GTSL::TupleElement<0, TTT>::type, GTSL::float32>());

static_assert(GTSL::GetTypeIndex<GTSL::float32, GTSL::float32, GTSL::uint32, GTSL::uint8, GTSL::uint16, GTSL::uint64>() == 0);
static_assert(GTSL::GetTypeIndex<GTSL::uint64, GTSL::float32, GTSL::uint32, GTSL::uint8, GTSL::uint16, GTSL::uint64>() == 4);

static_assert(GTSL::IsSame<GTSL::TypeAt<0, GTSL::uint64, GTSL::float32, GTSL::uint32, GTSL::uint8, GTSL::uint16, GTSL::uint64>::type, GTSL::uint64>());
static_assert(GTSL::IsSame<GTSL::TypeAt<1, GTSL::uint64, GTSL::float32, GTSL::uint32, GTSL::uint8, GTSL::uint16, GTSL::uint64>::type, GTSL::float32>());
static_assert(GTSL::IsSame<GTSL::TypeAt<3, GTSL::uint64, GTSL::float32, GTSL::uint32, GTSL::uint8, GTSL::uint16, GTSL::uint64>::type, GTSL::uint8>());

static_assert(GTSL::PackSizeAt<0, GTSL::uint32, GTSL::uint32, GTSL::uint32, GTSL::uint32>() == 0);
static_assert(GTSL::PackSizeAt<1, GTSL::uint32, GTSL::uint32, GTSL::uint32, GTSL::uint32>() == 4);
static_assert(GTSL::PackSizeAt<2, GTSL::uint32, GTSL::uint32, GTSL::uint32, GTSL::uint32>() == 8);
static_assert(GTSL::PackSizeAt<3, GTSL::uint32, GTSL::uint32, GTSL::uint32, GTSL::uint32>() == 12);

TEST(Bit, FindFirstSet) {
	auto result = GTSL::FindFirstSetBit((GTSL::uint8)1);

	ASSERT_TRUE(result.State());
	GTEST_ASSERT_EQ(result.Get(), 0);
}

TEST(Bit, FindLastSet) {
	auto result = GTSL::FindLastSetBit(1u);

	ASSERT_TRUE(result.State());
	GTEST_ASSERT_EQ(result.Get(), 0);
}

TEST(Bit, FindFirstClear) {
	auto result = GTSL::FindFirstClearBit(1u);

	ASSERT_TRUE(result.State());
	GTEST_ASSERT_EQ(result.Get(), 1);
}

TEST(Bit, FindLastClear) {
	auto result = GTSL::FindLastClearBit(1u);

	ASSERT_TRUE(result.State());
	GTEST_ASSERT_EQ(result.Get(), 1);
}

TEST(Bit, FindFirstSetBit) {
	auto result = GTSL::FindFirstSetBit(4096u);

	ASSERT_TRUE(result.State());
	GTEST_ASSERT_EQ(result.Get(), 12u);
}