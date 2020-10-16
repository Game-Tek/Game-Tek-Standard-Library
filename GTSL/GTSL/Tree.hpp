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
		}

		Node* AddChild(Node* parent)
		{
			Node* newNode;
			uint64 allocatedSize;
			allocator.Allocate(sizeof(Node), alignof(Node), reinterpret_cast<void**>(&newNode), &allocatedSize);
			parent->Nodes.EmplaceBack(newNode);
			return newNode;
		}

		~Tree()
		{
			auto freeNode = [&](Node* node, auto&& self) -> void
			{
				for (uint32 i = 0; i < node->Nodes.GetLength(); ++i) { self(node->Nodes[i], self); }

				node->Data.~T();
				allocator.Deallocate(sizeof(Node), alignof(Node), node);
			};

			for(uint32 i = 0; i < root.Nodes.GetLength(); ++i)
			{
				freeNode(root.Nodes[i], freeNode);
			}
		}
		
		Node* GetRootNode() { return &root; }

		const Node* GetRootNode() const { return &root; }
		
	private:
		Node root;

		ALLOCATOR allocator;
	};
}
