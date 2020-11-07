#pragma once
#include "Core.h"
#include "Vector.hpp"

namespace GTSL
{
	template<typename T, class ALLOCATOR>
	class Tree
	{
	public:
		struct Node
		{
			T Data;
			Vector<struct Node*, ALLOCATOR> Nodes;
		};

		Tree() = default;

		void Initialize(const ALLOCATOR& allocator)
		{
			this->allocator = allocator;
			root = New<Node>(allocator);
			root->Nodes.Initialize(4, allocator);
		}

		Node* AddChild(Node* parent)
		{
			Node* newNode = New<Node>(allocator);
			newNode->Nodes.Initialize(4, allocator);
			parent->Nodes.EmplaceBack(newNode);
			return newNode;
		}

		~Tree()
		{
			auto freeNode = [&](Node* node, auto&& self) -> void
			{
				for (uint32 i = 0; i < node->Nodes.GetLength(); ++i) { self(node->Nodes[i], self); }

				Delete(node, allocator);
			};

			freeNode(root, freeNode);
		}
		
		Node* GetRootNode() { return root; }

		const Node* GetRootNode() const { return root; }
		
	private:
		Node* root;

		ALLOCATOR allocator;
	};
}
