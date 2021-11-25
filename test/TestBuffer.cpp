#include <gtest/gtest.h>

#include "GTSL/Buffer.hpp"
#include "GTSL/Serialize.hpp"

TEST(Buffer, Connstruct) {
	GTSL::Buffer<GTSL::DefaultAllocatorReference> buffer(1024, 8);

	buffer << 3 << 5 << 19;

	GTSL::Insert(3.141f, buffer);

	GTEST_ASSERT_EQ(buffer.GetSize(), 16);

	int a, b, c;
	GTSL::float32 d;

	buffer >> a >> b >> c;

	GTSL::Extract(d, buffer);

	GTEST_ASSERT_EQ(a, 3);
	GTEST_ASSERT_EQ(b, 5);
	GTEST_ASSERT_EQ(c, 19);
	GTEST_ASSERT_EQ(d, 3.141f);
}