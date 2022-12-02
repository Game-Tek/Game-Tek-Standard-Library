#include <gtest/gtest.h>

#include <GTSL/Tree.hpp>

#include "GTSL/System.hpp"

class DestructionTester {
	GTSL::uint32* d = nullptr;

public:
	DestructionTester(GTSL::uint32* d) : d(d) {}

	~DestructionTester() {
		++(*d);
	}
};

TEST(MultiTree, Construct) {
	GTSL::uint32 counter = 0;

	{
		GTSL::MultiTree<GTSL::DefaultAllocatorReference, GTSL::uint32, GTSL::uint32, DestructionTester> tree(4);
		tree.Emplace<DestructionTester>(0, 0, 0, &counter);
	}
	
	GTEST_ASSERT_EQ(counter, 1); //test destructor is run on object
}

TEST(MultiTree, Insertion) {
	using TreeType = GTSL::MultiTree<GTSL::DefaultAllocatorReference, GTSL::uint32, GTSL::uint32, GTSL::float32>;
	TreeType tree;

	auto top = tree.Emplace<GTSL::float32>(0, 0xFFFFFFFF, 0, 0.0f);
	auto left = tree.Emplace<GTSL::float32>(1, 0xFFFFFFFF, top.Get(), 1.0f);
	tree.Emplace<GTSL::float32>(3, 0xFFFFFFFF, top.Get(), 3.0f);

	tree.Emplace<GTSL::float32>(2, left.Get(), top.Get(), 2.0f); // Insertion in level, with left node reference

	auto existingKeyResult = tree.Emplace<GTSL::float32>(2, left.Get(), top.Get(), 18.0f); // Insertion in with existing key

	EXPECT_FALSE(existingKeyResult);

	{
		GTSL::StaticVector<GTSL::float32, 16> test;

		auto visitNode = [&](uint32_t node, GTSL::uint32 level) {
			switch (tree.GetNodeType(node)) {
			case TreeType::GetTypeIndex<GTSL::float32>():
				test.EmplaceBack(tree.GetClass<GTSL::float32>(node));
				break;
			}
		};

		auto endNode = [&](uint32_t value, GTSL::uint32 level) {
		};

		ForEach(tree, visitNode, endNode);

		GTSL::StaticVector<GTSL::float32, 16> testRes{ 0, 1, 2, 3 };
		GTEST_ASSERT_EQ(test.GetRange(), testRes.GetRange());
	}
}

TEST(MultiTree, Folding) {
	using TreeType = GTSL::MultiTree<GTSL::DefaultAllocatorReference, GTSL::uint32, GTSL::uint32, GTSL::float32>;
	TreeType tree;

	auto top = tree.Emplace<GTSL::float32>(0, 0xFFFFFFFF, 0, 0.0f);
	auto left = tree.Emplace<GTSL::float32>(1, 0xFFFFFFFF, top.Get(), 1.0f);
	auto right = tree.Emplace<GTSL::float32>(3, 0xFFFFFFFF, top.Get(), 3.0f);

	auto center = tree.Emplace<GTSL::float32>(2, left.Get(), top.Get(), 2.0f); // Insertion in level, with left node reference

	auto extra = tree.Emplace<GTSL::float32>(4, 0xFFFFFFFF, top.Get(), 32.f); // Node to merge
	auto extraLeft = tree.Emplace<GTSL::float32>(66, 0xFFFFFFFF, extra.Get(), 4.f); // Node to merge
	auto extraRight = tree.Emplace<GTSL::float32>(77, 0xFFFFFFFF, extra.Get(), 5.f); // Node to merge

	auto existingKeyResult = tree.Emplace<GTSL::float32>(2, left.Get(), top.Get(), 18.0f); // Insertion in with existing key

	tree.UpdateNodeKey(extra.Get(), 1);
	tree.UpdateNodeKey(extraLeft.Get(), 7);
	tree.UpdateNodeKey(extraRight.Get(), 7);

	tree.Optimize();

	EXPECT_FALSE(existingKeyResult);

	{
		GTSL::StaticVector<GTSL::float32, 16> test;

		auto visitNode = [&](uint32_t node, GTSL::uint32 level) {
			switch (tree.GetNodeType(node)) {
			case TreeType::GetTypeIndex<GTSL::float32>():
				test.EmplaceBack(tree.GetClass<GTSL::float32>(node));
				break;
			}
		};

		auto endNode = [&](uint32_t value, GTSL::uint32 level) {
		};

		ForEach(tree, visitNode, endNode);

		GTSL::StaticVector<GTSL::float32, 16> testRes{ 0.f, 1.f, 4.f, 2.f, 3.f };
		GTEST_ASSERT_EQ(test.GetRange(), testRes.GetRange());
	}
}

TEST(MultiTree, Removal) {

}

TEST(MultiTree, Iteration) {
	using TreeType = GTSL::MultiTree<GTSL::DefaultAllocatorReference, GTSL::uint32, GTSL::uint32, GTSL::float32>;

	TreeType tree(8);

	auto result = tree.Emplace<GTSL::float32>(0, 0xFFFFFFFF, 0, 0.0f);

	auto alphaRoot = result.Get();

	GTEST_ASSERT_EQ(tree.GetChildrenCount(alphaRoot), 0);

	auto lRoot = tree.Emplace<GTSL::float32>(1, 0xFFFFFFFF, alphaRoot, 1.0f);

	GTEST_ASSERT_EQ(tree.GetChildrenCount(alphaRoot), 1);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lRoot.Get()), 0);

	auto llRoot = tree.Emplace<GTSL::float32>(2, 0xFFFFFFFF, lRoot.Get(), 2.0f);

	GTEST_ASSERT_EQ(tree.GetChildrenCount(alphaRoot), 1);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lRoot.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(llRoot.Get()), 0);

	auto lrRoot = tree.Emplace<GTSL::float32>(3, 0xFFFFFFFF, lRoot.Get(), 3.0f);

	GTEST_ASSERT_EQ(tree.GetChildrenCount(alphaRoot), 1);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lRoot.Get()), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(llRoot.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lrRoot.Get()), 0);

	auto rRoot = tree.Emplace<GTSL::float32>(4, 0xFFFFFFFF, alphaRoot, 4.0f);

	GTEST_ASSERT_EQ(tree.GetChildrenCount(alphaRoot), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lRoot.Get()), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(llRoot.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lrRoot.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(rRoot.Get()), 0);

	auto rlRoot = tree.Emplace<GTSL::float32>(5, 0xFFFFFFFF, rRoot.Get(), 5.0f);

	GTEST_ASSERT_EQ(tree.GetChildrenCount(alphaRoot), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lRoot.Get()), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(llRoot.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lrRoot.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(rRoot.Get()), 1);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(rlRoot.Get()), 0);

	auto rrRoot = tree.Emplace<GTSL::float32>(6, 0xFFFFFFFF, rRoot.Get(), 6.0f);

	GTEST_ASSERT_EQ(tree.GetChildrenCount(alphaRoot), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lRoot.Get()), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(llRoot.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lrRoot.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(rRoot.Get()), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(rlRoot.Get()), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(rrRoot.Get()), 0);

	{
		GTSL::StaticVector<GTSL::float32, 16> test;

		auto visitNode = [&](uint32_t node, GTSL::uint32 level) {
			switch (tree.GetNodeType(node)) {
			case TreeType::GetTypeIndex<GTSL::float32>():
				test.EmplaceBack(tree.GetClass<GTSL::float32>(node));
				break;
			}
		};

		auto endNode = [&](uint32_t value, GTSL::uint32 level) {
		};

		ForEach(tree, visitNode, endNode);

		GTSL::StaticVector<GTSL::float32, 16> testRes{ 0, 1, 2, 3, 4, 5, 6 };
		GTEST_ASSERT_EQ(test.GetRange(), testRes.GetRange());
	}

	{ // Subtree iteration test
		GTSL::StaticVector<GTSL::float32, 16> test;

		auto visitNode = [&](uint32_t node, GTSL::uint32 level) {
			switch (tree.GetNodeType(node)) {
			case TreeType::GetTypeIndex<GTSL::float32>():
				test.EmplaceBack(tree.GetClass<GTSL::float32>(node));
				break;
			}
		};

		auto endNode = [&](uint32_t value, GTSL::uint32 level) {
		};

		ForEach(tree, visitNode, endNode, rRoot.Get());

		GTSL::StaticVector<GTSL::float32, 16> testRes{ 4, 5, 6 };
		GTEST_ASSERT_EQ(test.GetRange(), testRes.GetRange());
	}
}

TEST(Tree, Iteration) {
	GTSL::Tree<GTSL::uint32, GTSL::DefaultAllocatorReference> tree;

	auto root = tree.Emplace(0u, 0);
	auto l = tree.Emplace(root, 1);	auto r = tree.Emplace(root, 5);
	auto ll = tree.Emplace(l, 2); auto lr = tree.Emplace(l, 4); auto rl = tree.Emplace(r, 6); auto rr = tree.Emplace(r, 8);
	auto llc = tree.Emplace(ll, 3);	auto rlc = tree.Emplace(rl, 7);

	{
		GTSL::StaticVector<GTSL::uint32, 9> test;

		auto visitNode = [&](uint32_t node, uint32_t level) {
			test.EmplaceBack(node);
		};

		auto endNode = [&](uint32_t value, uint32_t level) {
		};

		ForEach(tree, visitNode, endNode);

		GTSL::StaticVector<GTSL::uint32, 9> testRes{ 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u };
		GTEST_ASSERT_EQ(test.GetRange(), testRes.GetRange());
	}

	{
		GTSL::StaticVector<GTSL::uint32, 9> test;
	
		auto visitLevel = [&](decltype(tree)::iterator iterator, auto&& self) -> void {
			test.EmplaceBack(iterator);
	
			for (auto e : iterator) {
				self(e, self);
			}
		};

		for(auto e : tree) {
			visitLevel(e, visitLevel);
		}
	
		GTSL::StaticVector<GTSL::uint32, 9> testRes{ 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u };
		GTEST_ASSERT_EQ(test.GetRange(), testRes.GetRange());
		
	}

	{
		GTSL::Tree<GTSL::uint32, GTSL::DefaultAllocatorReference> empTree;

		GTSL::StaticVector<GTSL::uint32, 9> test;
	
		auto visitLevel = [&](decltype(tree)::iterator iterator, auto&& self) -> void {
			test.EmplaceBack(iterator);
	
			for (auto e : iterator) {
				self(e, self);
			}
		};

		for(auto e : empTree) {
			visitLevel(e, visitLevel);
		}
	
		GTSL::StaticVector<GTSL::uint32, 9> testRes{};
		GTEST_ASSERT_EQ(test.GetRange(), testRes.GetRange());
	}

	//auto visitLevel1 = [&](decltype(tree.begin()) iterator, auto&& self) -> void {
	//	ok = ok && i++ == *iterator;
	//
	//	{
	//		auto b = iterator.begin();
	//
	//		for (GTSL::uint64 j = 0ull; j < iterator.GetLength(); ++j, ++b) {
	//			self(b, self);
	//		}
	//	}
	//};
	//
	//visitLevel1(tree.begin(), visitLevel1);
	//
	//ASSERT_TRUE(ok);
	//GTEST_ASSERT_EQ(i, 9);
}

TEST(Tree, PartialIteration) {
	//GTSL::Tree<GTSL::uint32, GTSL::DefaultAllocatorReference> tree;
	//
	//auto root = tree.Emplace(0u, 0);
	//auto l = tree.Emplace(root, 1);	auto r = tree.Emplace(root, 5);
	//auto ll = tree.Emplace(l, 2); auto lr = tree.Emplace(l, 4); auto rl = tree.Emplace(r, 6); auto rr = tree.Emplace(r, 8);
	//auto llc = tree.Emplace(ll, 3);	auto rlc = tree.Emplace(rl, 7);
	//
	//GTSL::uint32 i = 5; bool ok = true;
	//
	//auto visitLevel = [&](GTSL::Range<decltype(tree)::iterator> iterator, auto&& self) -> void {
	//	ok = ok && i++ == iterator.begin();
	//
	//	for (auto e : iterator.begin()) {
	//		self(GTSL::Range(e, iterator.end()), self);
	//	}
	//};
	//
	//visitLevel(GTSL::Range(tree.begin(r), tree.end()), visitLevel);
	//
	//ASSERT_TRUE(ok);
	//GTEST_ASSERT_EQ(i, 9);
}

TEST(Tree, Remove) {
	//GTSL::Tree<GTSL::uint32, GTSL::DefaultAllocatorReference> tree;
	//
	//auto root = tree.Emplace(0u, 0);
	//auto l = tree.Emplace(root, 1);	auto r = tree.Emplace(root, 5);
	//auto ll = tree.Emplace(l, 2); auto lr = tree.Emplace(l, 4); auto rl = tree.Emplace(r, 6); auto rr = tree.Emplace(r, 8);
	//auto llc = tree.Emplace(ll, 3);	auto rlc = tree.Emplace(rl, 7);
	//
	//tree.Remove(l);
	//
	//GTSL::uint32 i = 5; bool ok = true;
	//
	//auto visitLevel = [&](GTSL::Range<decltype(tree)::iterator> iterator, auto&& self) -> void {
	//	ok = ok && i++ == iterator.begin();
	//
	//	for (auto e : iterator.begin()) {
	//		self(GTSL::Range(e, iterator.end()), self);
	//	}
	//};
	//
	//visitLevel(GTSL::Range(tree.begin(r), tree.end()), visitLevel);
	//
	//ASSERT_TRUE(ok);
	//GTEST_ASSERT_EQ(i, 9);
}