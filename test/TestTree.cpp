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
	using TreeType = GTSL::AlphaBetaTree<GTSL::DefaultAllocatorReference, GTSL::uint32, GTSL::uint32, GTSL::float32>;

	TreeType tree(8);

	auto alphaRoot = tree.EmplaceAlpha(0, 11);

	auto betaRoot = tree.AddBeta(0, 0u, alphaRoot, 0.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 0);

	auto lRoot = tree.EmplaceAlpha(alphaRoot, 22);
	auto llRoot = tree.EmplaceAlpha(lRoot, 33);
	auto lrRoot = tree.EmplaceAlpha(lRoot, 44);
	auto rRoot = tree.EmplaceAlpha(alphaRoot, 55);
	auto rlRoot = tree.EmplaceAlpha(rRoot, 66);
	auto rrRoot = tree.EmplaceAlpha(rRoot, 77);

	auto bl0 = tree.AddBeta(1, alphaRoot, lRoot, 1.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bl0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 1);

	auto bl1 = tree.AddBeta(2, alphaRoot, lRoot, 2.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bl1.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bl0.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 1);

	auto bll0 = tree.AddBeta(3, lRoot, llRoot, 3.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bll0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bl1.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bl0.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 1);

	auto blr0 = tree.AddBeta(4, lRoot, lrRoot, 4.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(blr0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bll0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bl1.Get()), 2);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bl0.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 1);

	auto br0 = tree.AddBeta(5, alphaRoot, rRoot, 5.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(br0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 2);

	auto br1 = tree.AddBeta(6, alphaRoot, rRoot, 6.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(br1.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(br0.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 2);

	auto brl0 = tree.AddBeta(7, rRoot, rlRoot, 7.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(brl0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(br1.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(br0.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 2);

	auto brr0 = tree.AddBeta(8, rRoot, rrRoot, 8.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(brr0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(brl0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(br1.Get()), 2);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(br0.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 2);

	{
		GTSL::float32 i = 0;

		bool ok = true;

		auto visitNode = [&](uint32_t node, GTSL::uint32 level) {
			switch (tree.GetBetaNodeType(node)) {
			case TreeType::GetTypeIndex<GTSL::float32>():
				ok = ok && i++ == tree.GetClass<GTSL::float32>(node);
				break;
			}
		};

		auto endNode = [&](uint32_t value, GTSL::uint32 level) {
		};

		ForEachBeta(tree, visitNode, endNode);

		ASSERT_TRUE(ok);
		GTEST_ASSERT_EQ(i, 9u);
	}

	GTEST_ASSERT_EQ(tree.GetAlpha(alphaRoot), 11);

	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(alphaRoot, 0)), 0.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(lRoot, 0)), 1.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(lRoot, 1)), 2.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(lRoot, 2)), 2.f);

	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(rRoot, 0)), 5.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(rRoot, 1)), 6.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(rRoot, 2)), 6.f);

	GTSL::float32 shouldBeTree[] = { 0.0f, 1.0f, 5.0f, 6.0f, 7.0f, 8.0f };
	GTSL::float32 shouldBeInverse[] = { 1.0f, 7.0f, 8.0f, 6.0f, 5.0f, 0.f };

	tree.ToggleBranch(bl1.Get(), false);

	{
		GTSL::uint32 i = 0, j = 0;

		GTSL::uint32 levels[3]{ 0 };

		bool ok = true, okCount = true, okLevel = true;

		GTSL::uint32 deepestCall = 0;

		auto visitNode = [&](uint32_t node, GTSL::uint32 level) {
			switch (tree.GetBetaNodeType(node)) {
			case TreeType::GetTypeIndex<GTSL::float32>():
				ok = ok && shouldBeTree[i] == tree.GetClass<GTSL::float32>(node);
				break;
			}

			++i;

			++levels[level];

			GTSL::Max(&deepestCall, level);
		};

		auto endNode = [&](uint32_t node, GTSL::uint32 level) {			
			GTEST_ASSERT_EQ(shouldBeInverse[j], tree.GetClass<GTSL::float32>(node));

			++j;
			
			--levels[level];

			okCount = okCount && level <= deepestCall;
		};

		ForEachBeta(tree, visitNode, endNode);

		ASSERT_TRUE(ok);
		ASSERT_TRUE(okCount);
		GTEST_ASSERT_EQ(i, 6);
		GTEST_ASSERT_EQ(j, 6);

		for(auto e : levels) {
			GTEST_ASSERT_EQ(e, 0);
		}
	}
}

TEST(AlphaBetaTree, Iteration2) {
	using TreeType = GTSL::AlphaBetaTree<GTSL::DefaultAllocatorReference, GTSL::uint32, GTSL::uint32, GTSL::float32>;

	TreeType tree(8);

	auto alphaRoot = tree.EmplaceAlpha(0, 11);

	auto betaRoot = tree.AddBeta(0, 0u, alphaRoot, 0.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 0);

	auto lRoot = tree.EmplaceAlpha(alphaRoot, 22);
	auto rRoot = tree.EmplaceAlpha(alphaRoot, 55);

	auto rlRoot = tree.EmplaceAlpha(rRoot, 66);
	auto rrRoot = tree.EmplaceAlpha(rRoot, 77);

	auto llRoot = tree.EmplaceAlpha(lRoot, 33);
	auto lrRoot = tree.EmplaceAlpha(lRoot, 44);

	auto bl0 = tree.AddBeta(1, alphaRoot, lRoot, 1.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bl0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 1);

	auto bl1 = tree.AddBeta(2, alphaRoot, lRoot, 2.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bl1.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bl0.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 1);

	auto br0 = tree.AddBeta(5, alphaRoot, rRoot, 5.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(br0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 2);

	auto br1 = tree.AddBeta(6, alphaRoot, rRoot, 6.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(br1.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(br0.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 2);

	auto bll0 = tree.AddBeta(3, lRoot, llRoot, 3.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bll0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bl1.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bl0.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 2);

	auto blr0 = tree.AddBeta(4, lRoot, lrRoot, 4.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(blr0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bll0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bl1.Get()), 2);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(bl0.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 2);

	auto brl0 = tree.AddBeta(7, rRoot, rlRoot, 7.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(brl0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(br1.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(br0.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 2);

	auto brr0 = tree.AddBeta(8, rRoot, rrRoot, 8.0f);

	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(brr0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(brl0.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(br1.Get()), 2);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(br0.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetBetaNodeChildrenCount(betaRoot.Get()), 2);


	{
		GTSL::float32 i = 0;

		bool ok = true;

		auto visitNode = [&](uint32_t node, GTSL::uint32 level) {
			switch (tree.GetBetaNodeType(node)) {
			case TreeType::GetTypeIndex<GTSL::float32>():
				ok = ok && i++ == tree.GetClass<GTSL::float32>(node);
				break;
			}
		};

		auto endNode = [&](uint32_t value, GTSL::uint32 level) {
		};

		ForEachBeta(tree, visitNode, endNode);

		ASSERT_TRUE(ok);
		GTEST_ASSERT_EQ(i, 9u);
	}

	GTEST_ASSERT_EQ(tree.GetAlpha(alphaRoot), 11);

	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(alphaRoot, 0)), 0.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(lRoot, 0)), 1.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(lRoot, 1)), 2.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(lRoot, 2)), 2.f);

	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(rRoot, 0)), 5.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(rRoot, 1)), 6.f);
	GTEST_ASSERT_EQ(tree.GetClass<GTSL::float32>(tree.GetBetaHandleFromAlpha(rRoot, 2)), 6.f);

	GTSL::float32 shouldBeTree[] = { 0.0f, 1.0f, 5.0f, 6.0f, 7.0f, 8.0f };
	GTSL::float32 shouldBeInverse[] = { 1.0f, 7.0f, 8.0f, 6.0f, 5.0f, 0.f };

	tree.ToggleBranch(bl1.Get(), false);

	{
		GTSL::uint32 i = 0, j = 0;

		GTSL::uint32 levels[3]{ 0 };

		bool ok = true, okCount = true, okLevel = true;

		GTSL::uint32 deepestCall = 0;

		auto visitNode = [&](uint32_t node, GTSL::uint32 level) {
			switch (tree.GetBetaNodeType(node)) {
			case TreeType::GetTypeIndex<GTSL::float32>():
				ok = ok && shouldBeTree[i] == tree.GetClass<GTSL::float32>(node);
				break;
			}

			++i;

			++levels[level];

			GTSL::Max(&deepestCall, level);
		};

		auto endNode = [&](uint32_t node, GTSL::uint32 level) {
			GTEST_ASSERT_EQ(shouldBeInverse[j], tree.GetClass<GTSL::float32>(node));

			++j;

			--levels[level];

			okCount = okCount && level <= deepestCall;
		};

		ForEachBeta(tree, visitNode, endNode);

		ASSERT_TRUE(ok);
		ASSERT_TRUE(okCount);
		GTEST_ASSERT_EQ(i, 6);
		GTEST_ASSERT_EQ(j, 6);

		for (auto e : levels) {
			GTEST_ASSERT_EQ(e, 0);
		}
	}
}

TEST(Tree, Iteration) {
	GTSL::Tree<GTSL::uint32, GTSL::DefaultAllocatorReference> tree;

											auto root = tree.Emplace(0u, 0);
					auto l = tree.Emplace(root, 1);					auto r = tree.Emplace(root, 5);
					auto ll = tree.Emplace(l, 2); auto lr = tree.Emplace(l, 4); auto rl = tree.Emplace(r, 6); auto rr = tree.Emplace(r, 8);
					auto llc = tree.Emplace(ll, 3);								auto rlc = tree.Emplace(rl, 7);

	GTSL::uint32 i = 0;

	bool ok = true;

	auto visitNode = [&](uint32_t node, uint32_t level) {
		ok = ok && i++ == node;
	};

	auto endNode = [&](uint32_t value, uint32_t level) {
	};

	ForEach(tree, visitNode, endNode);

	ASSERT_TRUE(ok);
	GTEST_ASSERT_EQ(i, 9);
}