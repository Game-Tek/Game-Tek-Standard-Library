#include "GTSL/SmartPointer.hpp"

#include <gtest/gtest.h>

struct BaseClass{};
struct InheritedClass : BaseClass {};
struct OtherClass{};

TEST(SmartPointer, Build) {
	auto pointer = GTSL::SmartPointer<BaseClass, GTSL::DefaultAllocatorReference>(GTSL::DefaultAllocatorReference{});
}

TEST(SmartPointer, Move) {
	auto pointerA = GTSL::SmartPointer<BaseClass, GTSL::DefaultAllocatorReference>(GTSL::DefaultAllocatorReference{});
	GTSL::SmartPointer<BaseClass, GTSL::DefaultAllocatorReference> pointerB(GTSL::MoveRef(pointerA));
}

TEST(SmartPointer, MoveSubClass) {
	auto pointerA = GTSL::SmartPointer<InheritedClass, GTSL::DefaultAllocatorReference>(GTSL::DefaultAllocatorReference{});
	GTSL::SmartPointer<BaseClass, GTSL::DefaultAllocatorReference> pointerB(GTSL::MoveRef(pointerA));
}