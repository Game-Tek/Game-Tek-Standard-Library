#include <gtest/gtest.h>

#include "GTSL/Allocator.hpp"
#include "GTSL/Memory.h"

struct A {
	A() {
		GTSL::uint64 allocatedSize = 0;
		allocator.Allocate(128, 8, (void**)&data, &allocatedSize);

		for(GTSL::uint8 i = 0; i < 128; ++i) {
			data[i] = i;			
		}
	}

	A(const A& other) : allocator(other.allocator) {
		GTSL::uint64 allocatedSize = 0;
		allocator.Allocate(128, 8, (void**)&data, &allocatedSize);

		for (GTSL::uint8 i = 0; i < 128; ++i) {
			data[i] = other.data[i];
		}
	}

	//A(A&&) = delete;

	GTSL::byte* data;
	GTSL::StaticAllocator<128> allocator;
};

TEST(StaticAllocator, Move) {
	A start;
	A end((A&&)start);

	for(GTSL::uint8 i = 0; i < 128; ++i) {
		GTEST_ASSERT_EQ(end.data[i], i);
	}
}