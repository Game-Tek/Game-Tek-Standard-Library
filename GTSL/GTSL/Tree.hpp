#pragma once
#include "Core.h"
#include "Vector.hpp"

namespace GTSL
{
	template<typename T, class ALLOCATOR>
	class Tree
	{
	public:
		struct Node {
			T Data;
			Vector<struct Node*, ALLOCATOR> Nodes;

			template<typename... ARGS>
			Node(const ALLOCATOR& allocator, ARGS&&... args) : Data(GTSL::ForwardRef<ARGS>(args)...), Nodes(4, allocator) {}
		};

		Tree() = delete;

		Tree(const ALLOCATOR& alloc) : allocator(allocator), nodes(4, allocator)
		{
		}

		template<typename... ARGS>
		Node* AddChild(Node* parent, ARGS&&... args) {
			if (parent) {
				Node* newNode = New<Node>(allocator, allocator, GTSL::ForwardRef<ARGS>(args)...);
				parent->Nodes.EmplaceBack(newNode);
				return newNode;
			} else  {
				Node* newNode = New<Node>(allocator, allocator, GTSL::ForwardRef<ARGS>(args)...);
				nodes.EmplaceBack(newNode);
				return newNode;
			}
		}

		~Tree() {
			auto freeNode = [&](Node* node, auto&& self) -> void {
				for (uint32 i = 0; i < node->Nodes.GetLength(); ++i) { self(node->Nodes[i], self); }

				Delete(&node, allocator);
			};

			for(auto e : nodes) {
				freeNode(e, freeNode);
			}
		}

		Node* operator[](const uint32 i) { return nodes[i]; }
		const Node* operator[](const uint32 i) const { return nodes[i]; }

		auto begin() const { return nodes.begin(); }
		auto end() const { return nodes.end(); }
	
	private:
		ALLOCATOR allocator;
		Vector<Node*, ALLOCATOR> nodes;
	};
}
