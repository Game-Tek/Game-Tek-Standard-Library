#include "GTSL/SmartPointer.hpp"
#include "GTSL/Delegate.hpp"

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

TEST(SmartPointer, BuildFromFunctionPointer) {
	using Delegate = GTSL::Delegate<void(void*, int, float*)>;
	auto pointerA = GTSL::SmartPointer<Delegate, GTSL::DefaultAllocatorReference>(GTSL::DefaultAllocatorReference{});
	GTSL::SmartPointer<Delegate, GTSL::DefaultAllocatorReference> pointerB(GTSL::MoveRef(pointerA));
}