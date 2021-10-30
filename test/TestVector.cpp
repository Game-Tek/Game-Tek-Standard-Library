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

TEST(Vector, Length) {
	GTSL::Vector<GTSL::uint32, GTSL::DefaultAllocatorReference> vector(2);

	//Test bool operator
	ASSERT_FALSE(vector);
	//Test int comparison operator
	ASSERT_FALSE(0u < vector);

	vector.EmplaceBack(0);

	//Test bool operator
	ASSERT_TRUE(vector);
	//Test int comparison operator
	ASSERT_TRUE(0u < vector);
}

TEST(MultiVectorAOS, Construct){
	GTSL::MultiVector<GTSL::DefaultAllocatorReference, true, GTSL::uint8, GTSL::uint16, GTSL::uint32, GTSL::float32> multiVector;
}

TEST(MultiVectorAOS, Insert) {
	GTSL::MultiVector<GTSL::DefaultAllocatorReference, true, GTSL::uint8, GTSL::uint16, GTSL::uint32, GTSL::float32> multiVector;

	multiVector.EmplaceBack(0, 1, 2, 3.f);

	GTEST_ASSERT_EQ(multiVector.At<0>(0), 0);
	GTEST_ASSERT_EQ(multiVector.At<1>(0), 1);
	GTEST_ASSERT_EQ(multiVector.At<2>(0), 2);
	GTEST_ASSERT_EQ(multiVector.At<3>(0), 3.f);

	multiVector.EmplaceBack(4, 5, 6, 7.f);

	GTEST_ASSERT_EQ(multiVector.At<0>(1), 4);
	GTEST_ASSERT_EQ(multiVector.At<1>(1), 5);
	GTEST_ASSERT_EQ(multiVector.At<2>(1), 6);
	GTEST_ASSERT_EQ(multiVector.At<3>(1), 7.f);
}

TEST(MultiVectorSOA, Insert) {
	GTSL::MultiVector<GTSL::DefaultAllocatorReference, false, GTSL::uint8, GTSL::uint16, GTSL::uint32, GTSL::float32> multiVector;

	multiVector.EmplaceBack(0, 1, 2, 3.f);

	GTEST_ASSERT_EQ(multiVector.At<0>(0), 0);
	GTEST_ASSERT_EQ(multiVector.At<1>(0), 1);
	GTEST_ASSERT_EQ(multiVector.At<2>(0), 2);
	GTEST_ASSERT_EQ(multiVector.At<3>(0), 3.f);

	GTEST_ASSERT_EQ(*multiVector.GetPointer<0>(0), 0);
	GTEST_ASSERT_EQ(*multiVector.GetPointer<1>(0), 1);
	GTEST_ASSERT_EQ(*multiVector.GetPointer<2>(0), 2);
	GTEST_ASSERT_EQ(*multiVector.GetPointer<3>(0), 3.f);

	multiVector.EmplaceBack(4, 5, 6, 7.f);

	GTEST_ASSERT_EQ(multiVector.At<0>(1), 4);
	GTEST_ASSERT_EQ(multiVector.At<1>(1), 5);
	GTEST_ASSERT_EQ(multiVector.At<2>(1), 6);
	GTEST_ASSERT_EQ(multiVector.At<3>(1), 7.f);

	GTEST_ASSERT_EQ(*multiVector.GetPointer<0>(1), 4);
	GTEST_ASSERT_EQ(*multiVector.GetPointer<1>(1), 5);
	GTEST_ASSERT_EQ(*multiVector.GetPointer<2>(1), 6);
	GTEST_ASSERT_EQ(*multiVector.GetPointer<3>(1), 7.f);
}