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

TEST(MultiTree, Construct) {
	GTSL::uint32 counter = 0;

	{
		GTSL::MultiTree<GTSL::DefaultAllocatorReference, uint32_t, uint32_t, DestructionTester> tree(4);		
		tree.Emplace<DestructionTester>(0, 0, &counter);
	}
	
	GTEST_ASSERT_EQ(counter, 1); //test destructor is run on object
}

TEST(MultiTree, Insertion) {
	using TreeType = GTSL::MultiTree<GTSL::DefaultAllocatorReference, GTSL::uint32, GTSL::uint32, GTSL::float32>;
	TreeType tree;

	auto top = tree.Emplace<GTSL::float32>(0xFFFFFFFF, 0, 0.0f);
	auto left = tree.Emplace<GTSL::float32>(0xFFFFFFFF, top, 1.0f);
	tree.Emplace<GTSL::float32>(0xFFFFFFFF, top, 3.0f);

	tree.Emplace<GTSL::float32>(left, top, 2.0f);

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

TEST(MultiTree, Removal) {

}

TEST(MultiTree, Iteration) {
	using TreeType = GTSL::MultiTree<GTSL::DefaultAllocatorReference, GTSL::uint32, GTSL::uint32, GTSL::float32>;

	TreeType tree(8);

	auto alphaRoot = tree.Emplace<GTSL::float32>(0xFFFFFFFF, 0, 0.0f);

	GTEST_ASSERT_EQ(tree.GetChildrenCount(alphaRoot), 0);

	auto lRoot = tree.Emplace<GTSL::float32>(0xFFFFFFFF, alphaRoot, 1.0f);

	GTEST_ASSERT_EQ(tree.GetChildrenCount(alphaRoot), 1);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lRoot), 0);

	auto llRoot = tree.Emplace<GTSL::float32>(0xFFFFFFFF, lRoot, 2.0f);

	GTEST_ASSERT_EQ(tree.GetChildrenCount(alphaRoot), 1);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lRoot), 1);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(llRoot), 0);

	auto lrRoot = tree.Emplace<GTSL::float32>(0xFFFFFFFF, lRoot, 3.0f);

	GTEST_ASSERT_EQ(tree.GetChildrenCount(alphaRoot), 1);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lRoot), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(llRoot), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lrRoot), 0);

	auto rRoot = tree.Emplace<GTSL::float32>(0xFFFFFFFF, alphaRoot, 4.0f);

	GTEST_ASSERT_EQ(tree.GetChildrenCount(alphaRoot), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lRoot), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(llRoot), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lrRoot), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(rRoot), 0);

	auto rlRoot = tree.Emplace<GTSL::float32>(0xFFFFFFFF, rRoot, 5.0f);

	GTEST_ASSERT_EQ(tree.GetChildrenCount(alphaRoot), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lRoot), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(llRoot), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lrRoot), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(rRoot), 1);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(rlRoot), 0);

	auto rrRoot = tree.Emplace<GTSL::float32>(0xFFFFFFFF, rRoot, 6.0f);

	GTEST_ASSERT_EQ(tree.GetChildrenCount(alphaRoot), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lRoot), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(llRoot), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(lrRoot), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(rRoot), 2);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(rlRoot), 0);
	GTEST_ASSERT_EQ(tree.GetChildrenCount(rrRoot), 0);

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

		ForEach(tree, visitNode, endNode, rRoot);

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

	//{
	//	GTSL::StaticVector<GTSL::uint32, 9> test;
	//
	//	auto visitLevel = [&](decltype(tree)::iterator iterator, auto&& self) -> void {
	//		test.EmplaceBack(iterator);
	//
	//		for (auto e : iterator) {
	//			self(e, self);
	//		}
	//	};
	//
	//	visitLevel(tree.begin(), visitLevel);
	//
	//	GTSL::StaticVector<GTSL::uint32, 9> testRes{ 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u };
	//	GTEST_ASSERT_EQ(test.GetRange(), testRes.GetRange());
	//	
	//}

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