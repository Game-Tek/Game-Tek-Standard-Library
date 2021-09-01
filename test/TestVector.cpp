#include <gtest/gtest.h>
#include <GTSL/Vector.hpp>

TEST(Vector, Construct) {
	GTSL::Vector<GTSL::uint32, GTSL::DefaultAllocatorReference> vector(18);

	ASSERT_GE(vector.GetCapacity(), 18);
	ASSERT_EQ(vector.GetLength(), 0);
	ASSERT_NE(vector.GetData(), nullptr);
}

TEST(Vector, EmplaceBack) {
	GTSL::Vector<GTSL::uint32, GTSL::DefaultAllocatorReference> vector(18);

	vector.EmplaceBack(32);
	vector.EmplaceBack(19);
	vector.EmplaceBack(24822);
	vector.EmplaceBack(68);

	ASSERT_EQ(vector[0], 32); ASSERT_EQ(vector[1], 19); ASSERT_EQ(vector[2], 24822); ASSERT_EQ(vector[3], 68);

	ASSERT_GE(vector.GetCapacity(), 18);
	ASSERT_EQ(vector.GetLength(), 4);
}

TEST(Vector, PopBack) {
	GTSL::Vector<GTSL::uint32, GTSL::DefaultAllocatorReference> vector(18);

	vector.EmplaceBack(32); vector.EmplaceBack(19); vector.EmplaceBack(24822); vector.EmplaceBack(68);

	vector.PopBack();

	ASSERT_EQ(vector[0], 32); ASSERT_EQ(vector[1], 19); ASSERT_EQ(vector[2], 24822);

	ASSERT_GE(vector.GetCapacity(), 18);
	ASSERT_EQ(vector.GetLength(), 3);
}

TEST(Vector, Pop) {
	GTSL::Vector<GTSL::uint32, GTSL::DefaultAllocatorReference> vector(18);

	vector.EmplaceBack(32); vector.EmplaceBack(19); vector.EmplaceBack(24822); vector.EmplaceBack(68);

	vector.Pop(0);

	ASSERT_EQ(vector[0], 19); ASSERT_EQ(vector[1], 24822); ASSERT_EQ(vector[2], 68);

	ASSERT_GE(vector.GetCapacity(), 18);
	ASSERT_EQ(vector.GetLength(), 3);

	vector.Pop(1);
	
	ASSERT_EQ(vector[0], 19); ASSERT_EQ(vector[1], 68);

	ASSERT_GE(vector.GetCapacity(), 18);
	ASSERT_EQ(vector.GetLength(), 2);
	
	vector.Pop(1);
	
	ASSERT_EQ(vector[0], 19);

	ASSERT_GE(vector.GetCapacity(), 18);
	ASSERT_EQ(vector.GetLength(), 1);
	
	vector.Pop(0);

	ASSERT_GE(vector.GetCapacity(), 18);
	ASSERT_EQ(vector.GetLength(), 0);
}