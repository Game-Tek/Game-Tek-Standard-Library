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
			nodes.Initialize(4, this->allocator);
		}

		Node* AddChild(Node* parent)
		{
			if (parent)
			{
				Node* newNode = New<Node>(allocator);
				newNode->Nodes.Initialize(4, allocator);
				parent->Nodes.EmplaceBack(newNode);
				return newNode;
			}
			else
			{
				Node* newNode = New<Node>(allocator);
				newNode->Nodes.Initialize(4, allocator);
				nodes.EmplaceBack(newNode);
				return newNode;
			}
		}

		~Tree()
		{
			auto freeNode = [&](Node* node, auto&& self) -> void
			{
				for (uint32 i = 0; i < node->Nodes.GetLength(); ++i) { self(node->Nodes[i], self); }

				Delete(node, allocator);
			};

			for(auto e : nodes) {
				freeNode(e, freeNode);
			}
		}

		Node* operator[](const uint32 i) { return nodes[i]; }
		const Node* operator[](const uint32 i) const { return nodes[i]; }
	
	private:
		GTSL::Vector<Node*, ALLOCATOR> nodes;

		ALLOCATOR allocator;
	};
}
