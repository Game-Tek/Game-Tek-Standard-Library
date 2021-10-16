#include <gtest/gtest.h>

#include <GTSL/Tree.hpp>

#include "GTSL/System.h"

class DestructionTester {
	GTSL::uint32* d = nullptr;

public:
	DestructionTester(GTSL::uint32* d) : d(d) {}

	~DestructionTester() {
		++(*d);
	}
};

TEST(AlphaBetaTree, Construct) {
	GTSL::uint32 counter = 0;

	{
		GTSL::AlphaBetaTree<GTSL::DefaultAllocatorReference, uint32_t, DestructionTester> tree(4);
		tree.EmplaceBeta<DestructionTester>(0, &counter);
	}
	
	GTEST_ASSERT_EQ(counter, 1); //test destructor is run on object
}

TEST(AlphaBetaTree, Insertion) {
	GTSL::AlphaBetaTree<GTSL::DefaultAllocatorReference, uint32_t, uint32_t> tree;
	
	auto handle = tree.AddBeta(0, 55u);
	GTEST_ASSERT_EQ(tree.At<GTSL::uint32>(handle), 55);
	
	GTEST_ASSERT_EQ(tree.At<uint32_t>(tree.AddBeta(handle, 66u)), 66u);
	GTEST_ASSERT_EQ(tree.At<uint32_t>(tree.AddBeta(handle, 77u)), 77u);
	GTEST_ASSERT_EQ(tree.At<uint32_t>(tree.AddBeta(handle, 88u)), 88u);
	GTEST_ASSERT_EQ(tree.At<uint32_t>(tree.AddBeta(handle, 99u)), 99u);
}

TEST(AlphaBetaTree, Removal) {

}

TEST(AlphaBetaTree, Iteration) {
	GTSL::AlphaBetaTree<GTSL::DefaultAllocatorReference, GTSL::uint32, GTSL::float32> tree(2);

	auto alphaRoot = tree.EmplaceAlpha(0, 36);

	auto root = tree.AddBeta(0u, 0.0f);
	tree.AddBeta(tree.AddBeta(tree.AddBeta(root, 1.0f), 2.0f), 3.0f);
	tree.AddBeta(tree.AddBeta(tree.AddBeta(root, 4.0f), 5.0f), 6.0f);

	GTSL::float32 i = 0;

	bool ok = true;

	auto visitNode = [&](uint32_t node) {
		ok = ok && i++ == tree.At<GTSL::float32>(node);
	};

	auto endNode = [&](uint32_t value) {
	};

	ForEachBeta(tree, visitNode, endNode);

	ASSERT_TRUE(ok);

	GTEST_ASSERT_EQ(tree.At(alphaRoot), 36);
}

TEST(Tree, Iteration) {
	GTSL::Tree<GTSL::uint32, GTSL::DefaultAllocatorReference> tree;

	auto root = tree.Emplace(0u, 0);
	tree.Emplace(tree.Emplace(tree.Emplace(root, 1), 2), 3);
	tree.Emplace(tree.Emplace(tree.Emplace(root, 4), 5), 6);

	GTSL::uint32 i = 0;

	bool ok = true;

	auto visitNode = [&](uint32_t node) {
		ok = ok && i++ == tree[node];
	};

	auto endNode = [&](uint32_t value) {
	};

	ForEach(tree, visitNode, endNode);

	ASSERT_TRUE(ok);
}