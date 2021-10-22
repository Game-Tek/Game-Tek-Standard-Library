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
		GTSL::AlphaBetaTree<GTSL::DefaultAllocatorReference, uint32_t, uint32_t, DestructionTester> tree(4);		
		tree.EmplaceBeta<DestructionTester>(0, 0, tree.EmplaceAlpha(0, 222), GTSL::MoveRef(&counter));
	}
	
	GTEST_ASSERT_EQ(counter, 1); //test destructor is run on object
}

TEST(AlphaBetaTree, Insertion) {
	GTSL::AlphaBetaTree<GTSL::DefaultAllocatorReference, uint32_t, uint32_t> tree;
	
	//auto handle = tree.AddBeta(0, 55u);
	//GTEST_ASSERT_EQ(tree.At<GTSL::uint32>(handle), 55);
	//
	//GTEST_ASSERT_EQ(tree.At<uint32_t>(tree.AddBeta(handle, 66u)), 66u);
	//GTEST_ASSERT_EQ(tree.At<uint32_t>(tree.AddBeta(handle, 77u)), 77u);
	//GTEST_ASSERT_EQ(tree.At<uint32_t>(tree.AddBeta(handle, 88u)), 88u);
	//GTEST_ASSERT_EQ(tree.At<uint32_t>(tree.AddBeta(handle, 99u)), 99u);
}

TEST(AlphaBetaTree, Removal) {

}

TEST(AlphaBetaTree, Iteration) {
	GTSL::AlphaBetaTree<GTSL::DefaultAllocatorReference, GTSL::uint32, GTSL::uint32, GTSL::float32> tree(8);

	auto alphaRoot = tree.EmplaceAlpha(0, 11);

	tree.AddBeta(0, 0u, alphaRoot, 0.0f);

	auto lRoot = tree.EmplaceAlpha(alphaRoot, 22);
	auto llRoot = tree.EmplaceAlpha(lRoot, 33);
	auto lrRoot = tree.EmplaceAlpha(lRoot, 44);
	auto rRoot = tree.EmplaceAlpha(alphaRoot, 55);
	auto rlRoot = tree.EmplaceAlpha(rRoot, 66);
	auto rrRoot = tree.EmplaceAlpha(rRoot, 77);

	tree.AddBeta(1, alphaRoot, lRoot, 1.0f);
	tree.AddBeta(2, alphaRoot, lRoot, 2.0f);
	tree.AddBeta(3, alphaRoot, llRoot, 3.0f);
	tree.AddBeta(4, alphaRoot, lrRoot, 4.0f);

	tree.AddBeta(5, alphaRoot, rRoot, 5.0f);
	tree.AddBeta(6, alphaRoot, rRoot, 6.0f);
	tree.AddBeta(7, alphaRoot, rlRoot, 7.0f);	
	tree.AddBeta(8, alphaRoot, rrRoot, 8.0f);

	GTSL::float32 i = 0;

	bool ok = true;

	auto visitNode = [&](uint32_t node) {
		ok = ok && i++ == tree.GetClass<GTSL::float32>(node);
	};

	auto endNode = [&](uint32_t value) {
	};

	ForEachBeta(tree, visitNode, endNode);

	ASSERT_TRUE(ok);

	GTEST_ASSERT_EQ(tree.GetAlpha(alphaRoot), 11);

	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(alphaRoot, 0)), 0.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(lRoot, 0)), 1.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(lRoot, 1)), 2.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(lRoot, 2)), 2.f);

	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(rRoot, 0)), 5.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(rRoot, 1)), 6.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(rRoot, 2)), 6.f);
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