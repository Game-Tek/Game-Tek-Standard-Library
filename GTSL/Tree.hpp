#pragma once
#include "Core.h"
#include "HashMap.hpp"
#include "System.hpp"
#include "Vector.hpp"
#include "Tuple.hpp"
#include "Algorithm.hpp"

namespace GTSL
{
	template<typename CLASS, uint64 INDEX>
	bool Destroy(uint32 index, void* p) {
		if (index == INDEX) { Destroy(*static_cast<CLASS*>(p)); return true; }
		return false;
	}

	template<typename... CLASSES, uint64... Is>
	void Destroy(uint32 index, void* p, Indices<Is...>) {
		(Destroy<CLASSES, Is>(index, p), ...);
	}

	template<typename... CLASSES>
	void Destroy(uint32 index, void* p) {
		Destroy<CLASSES...>(index, p, BuildIndices<sizeof...(CLASSES)>{});
	}

	template<typename CLASS, uint64 INDEX>
	bool Move(void* from, void* to, uint32 index) {
		if (index == INDEX) { Move(static_cast<CLASS*>(from), static_cast<CLASS*>(to)); return true; }
		return false;
	}

	template<typename... CLASSES, uint64... Is>
	void Move(void* from, void* to, uint32 index, Indices<Is...>) {
		(Move<CLASSES, Is>(from, to, index), ...);
	}

	template<typename... CLASSES>
	void Move(void* from, void* to, uint32 index) {
		Move<CLASSES...>(from, to, index, BuildIndices<sizeof...(CLASSES)>{});
	}

	template<typename T, class ALLOCATOR>
	class Tree {
		struct Node {
			T Data;
			uint32 TreeDown = 0, TreeRight = 0, ChildrenCount = 0, RightMostChild = 0;

			template<typename ... ARGS>
			Node(ARGS&&... args) : Data(GTSL::ForwardRef<ARGS>(args)...) {}
		};
	public:
		using Key = uint32;

		Tree(const ALLOCATOR& allocator = ALLOCATOR()) : allocator(allocator) {}
		Tree(const uint32 minElements, const ALLOCATOR& allocator = ALLOCATOR()) : allocator(allocator) {}

		template<typename... ARGS>
		uint32 Emplace(const uint32 parentNodeHandle, ARGS&&... args) {
			tryResize(1);

			::new(nodes + length++) Node(GTSL::ForwardRef<ARGS>(args)...);

			if(parentNodeHandle) {
				Node& parentNode = at(parentNodeHandle);

				if (!parentNode.TreeDown) { parentNode.TreeDown = length; }

				if (parentNode.ChildrenCount) { //write "to the right" if there are elements to our left
					at(parentNode.RightMostChild).TreeRight = length;
				}

				parentNode.ChildrenCount += 1;
				parentNode.RightMostChild = length;
			}

			return length;
		}

		void Remove(const uint32 node_handle) {
			auto destroyNode = [&](iterator handle, auto&&self) -> void {
				for(auto e : handle) {
					self(e, self);
				}

				GTSL::Destroy(static_cast<T&>(handle));
			};

			destroyNode(begin(node_handle), destroyNode);
		}

		T& operator[](const Key handle) { return at(handle).Data; }
		const T& operator[](const Key handle) const { return at(handle).Data; }
		
		struct iterator {
			iterator(Node* d, uint32 l, uint32 p) : data(d), level(l), pos(p) {}

			void operator++() {
				pos = data[pos - 1].TreeRight;
			}

			//bool operator<(const iterator& other) {
			//	return pos < other.pos;
			//}

			bool operator!=(const iterator& other) const { return pos != other.pos; }

			iterator operator*() { return { data, level, pos }; }
			operator T&() { return data[pos - 1].Data; }
			operator const T&() const { return data[pos - 1].Data; }

			uint32 GetLevel() const { return level; }
			uint32 GetLength() const { return data[pos - 1].ChildrenCount; }
			uint32 GetHandle() const { return pos; }

			[[nodiscard]] auto begin() { return iterator{ data, level + 1, data[pos - 1].TreeDown }; }
			[[nodiscard]] auto end() { return iterator{ data, level + 1, 0 }; }

		private:
			Node* data;
			uint32 level;
			uint32 pos = 0;
		};
		
		using const_iterator = iterator;

		iterator begin(const uint32 i = 1) { return iterator{ nodes, 0, length ? i : 0 }; }
		iterator end(const uint32 i = 0) { return iterator{ nodes, 0, 0 }; }

		[[nodiscard]] const_iterator begin() const { return iterator{ nodes, 0, 1 }; }
		[[nodiscard]] const_iterator end() const { return iterator{ nodes, 0, 0 }; }

		operator Range<iterator>() { return Range<iterator>(begin(), end()); }
		operator Range<const_iterator>() const { return Range<const_iterator>(begin(), end()); }

		friend void ForEach(const Tree& tree, auto&& a, auto&& b) {
			if (!tree.length) { return; }

			Key next = 1;

			auto visitNode = [&](Key handle, uint32 level, auto&& self) -> void {
				const auto& node = tree.at(handle);
				
				a(tree[handle], level);

				next = node.TreeDown ? node.TreeDown : node.TreeRight;

				for (uint32 i = 0; i < node.ChildrenCount; ++i) {
					self(next, level + 1, self);
				}

				b(tree[handle], level);

				next = node.TreeRight;
			};

			visitNode(next, 0, visitNode);
			
		}

	private:
		ALLOCATOR allocator;
		Node* nodes = nullptr; uint32 length = 0, capacity = 0;

		Node& at(uint32 handle) { return nodes[handle - 1]; }
		const Node& at(uint32 handle) const { return nodes[handle - 1]; }

		void tryResize(const uint32 delta) {
			if (length + delta > capacity) {
				if (nodes) {
					Resize(allocator, &nodes, &capacity, capacity * 2, length);
				} else {
					Allocate(allocator, delta, &nodes, &capacity);
				}
			}
		}
	};

	template<class ALLOCATOR, typename ALPHA, typename... CLASSES>
	class MultiTree {
	public:
		using Key = uint32;
		using Types = Tuple<CLASSES...>;

		struct AlphaNode {
			template<typename... ARGS>
			AlphaNode(ARGS&&... args) : Alpha(ForwardRef<ARGS>(args)... ) {}

			uint64 NodeKey = ~0ULL;
			Key Parent = 0;
			bool Mergeable = true;
			ALPHA Alpha;
		};

		struct TraversalData {
			uint32 TreeDown = 0, TreeRight = 0;
			uint8 TypeIndex = 0;
			bool Way = true;
		};

		MultiTree(const ALLOCATOR& alloc = ALLOCATOR()) : allocator(alloc), outOfOrderNodes(16, allocator) {}

		MultiTree(const uint32 minNodes, const ALLOCATOR& alloc = ALLOCATOR()) : allocator(alloc), outOfOrderNodes(16, allocator) {
			tryResizeAlphaTable(minNodes);
			tryResizeMultiTable(minNodes * 32);
		}

		template<typename T, typename... ARGS>
		GTSL::Result<Key> Emplace(uint64 key, Key leftNodeHandle, Key parentNodeHandle, ARGS&&... params) {
			static_assert(IsInPack<T, CLASSES...>(), "Type T is not in tree's type list.");

			auto typeIndex = static_cast<uint8>(GetTypeIndex<T>());

			// Check if node key already exists, if so, return existing node and don't emplace a new one
			if(parentNodeHandle) {
				TraversalData tD = getTraversalData(parentNodeHandle);
				Key l = tD.TreeDown;

				while(l) {
					TraversalData p = getTraversalData(l);

					if(p.TypeIndex == typeIndex) { // If current node is of the same type as we are checking
						if(AlphaNode& t = getHelper(l); t.NodeKey == key) {
							return { GTSL::MoveRef(l), false };
						}
					}

					l = p.TreeRight;
				}
			}

			tryResizeAlphaTable(1);

			auto index = entries++;
			auto handleValue = entries;

			AlphaNode* alphaNode = ::new(alphaTable + index) AlphaNode();

			alphaNode->NodeKey = key;

			tryResizeMultiTable(sizeof(TraversalData) + sizeof(T));

			indirectionTable[index] = multiTableSize;

			TraversalData& entry = *::new(multiTable + multiTableSize) TraversalData();
			entry.TypeIndex = typeIndex;

			multiTableSize += sizeof(TraversalData);

			::new(multiTable + multiTableSize) T(GTSL::ForwardRef<ARGS>(params)...);

			multiTableSize += sizeof(T);

			if(parentNodeHandle) {
				alphaNode->Parent = parentNodeHandle;

				AlphaNode& alphaParent = getHelper(parentNodeHandle);
				TraversalData& parentNodeTraversalData = getTraversalData(parentNodeHandle);

				if (leftNodeHandle) {
					if(leftNodeHandle == 0xFFFFFFFF) {
						if (parentNodeTraversalData.TreeDown) { // If has children
							auto handle = parentNodeTraversalData.TreeDown;

							while (auto e = getTraversalData(handle).TreeRight) {
								handle = e;
							}

							getTraversalData(handle).TreeRight = handleValue;
						} else {
							parentNodeTraversalData.TreeDown = handleValue;							
						}
					} else {
						auto handle = getTraversalData(leftNodeHandle).TreeRight;
						getTraversalData(handleValue).TreeRight = handle;
						getTraversalData(leftNodeHandle).TreeRight = handleValue;
					}
				} else {
					getTraversalData(handleValue).TreeRight = parentNodeTraversalData.TreeDown;
					parentNodeTraversalData.TreeDown = handleValue;
				}

				//if(currentNodeHandle != hand + 1) {
				//	outOfOrderNodes.EmplaceBack(currentNodeHandle);
				//}

				//parentNodeTraversalData.ChildrenCount += 1;
			}

			return { GTSL::MoveRef(handleValue), true };
		}

		void UpdateNodeKey(Key node_handle, const uint64 new_key) {
			AlphaNode& node = getHelper(node_handle);

			node.NodeKey = new_key;
		}

		void SetIsMergeable(Key node_handle, bool is_mergeable) {
			getHelper(node_handle).Mergeable = is_mergeable;
		}

		void ToggleBranch(Key betaNodeBranchTop, bool toggle) {
			TraversalData& branchNode = getTraversalData(betaNodeBranchTop);
			branchNode.Way = toggle;
		}

		template<typename T>
		static consteval uint32 GetTypeIndex() {
			return GTSL::GetTypeIndex<T, CLASSES...>();
		}

		ALPHA& GetAlpha(const Key i) { return alphaTable[i - 1].Alpha; }
		[[nodiscard]] const ALPHA& GetAlpha(const Key i) const { return alphaTable[i - 1].Alpha; }

		[[nodiscard]] Key GetNodeParent(const Key key) const { return getHelper(key).AlphaParent; }

		//[[nodiscard]] uint32 GetChildrenCount(const Key betaNodeHandle) const { return getTraversalData(betaNodeHandle).ChildrenCount; }

		[[nodiscard]] uint32 GetChildrenCount(const Key betaNodeHandle) const {
			uint32 count = 0;
			const TraversalData& td = getTraversalData(betaNodeHandle);

			Key p = td.TreeDown;

			while(p) {
				++count;
				p = getTraversalData(p).TreeRight;
			}

			return count;
		}

		[[nodiscard]] bool GetNodeState(const Key node_handle) const { return getTraversalData(node_handle).Way; }

		[[nodiscard]] uint32 GetNodeType(const Key handle) const { return getTraversalData(handle).TypeIndex; }

		[[nodiscard]] uint32 GetAlphaLength() const { return entries; }

		template<typename T>
		T& GetClass(const Key nodeHandle) {
#if _DEBUG
			if (!(GetTypeIndex<T>() == getTraversalData(nodeHandle).TypeIndex)) { __debugbreak(); }
#endif
			return *reinterpret_cast<T*>(getClass(nodeHandle));
		}

		template<typename T>
		const T& GetClass(const Key nodeHandle) const {
#if _DEBUG
			if (!(GetTypeIndex<T>() == getTraversalData(nodeHandle).TypeIndex)) { __debugbreak(); }
#endif
			return *reinterpret_cast<const T*>(multiTable + getTraversalData(nodeHandle).Position + sizeof(T));
		}

		void Optimize() {
			//if (outOfOrderNodes.GetLength()) {
			//	Vector<Pair<uint32, uint32>, ALLOCATOR> groups;
			//	SortAscending(outOfOrderNodes);
			//	groups.EmplaceBack(1, outOfOrderNodes.front());
			//	for (uint32 i = 1; i < outOfOrderNodes.GetLength(); ++i) {
			//		if (outOfOrderNodes[i] == groups.back().Second + 1) { ++groups.back().First; } // If next node
			//		else { groups.EmplaceBack(1, outOfOrderNodes[i]); }
			//	}
			//	//do shifts
			//	outOfOrderNodes.Resize(0);
			//}

			auto visitNode = [&](Key handle, uint32 level, auto&& self) -> void {
				mergeNodes(handle, getHelper(handle).NodeKey);

				const TraversalData& node = getTraversalData(handle);
				auto next = node.TreeDown;

				while(next) {
					self(next, level + 1, self);
					next = getTraversalData(next).TreeRight;
				}
			};

			visitNode(1, 0, visitNode);
		}

		struct NodeReference {
			NodeReference(MultiTree* multi_tree) : multiTree(multi_tree) {}

			MultiTree* multiTree;
		};

		NodeReference GetNodeReference(const Key key) {
			return { this };
		}

	private:
		ALLOCATOR allocator;
		
		AlphaNode* alphaTable = nullptr; uint32 entries = 0, alphaCapacity = 0;
		uint32* indirectionTable = nullptr;
		byte* multiTable = nullptr; uint32 multiTableSize = 0, multiTableCapacity = 0;

		Vector<Key, ALLOCATOR> outOfOrderNodes;

		void removeNode(Key parent_node_handle, Key left_node_handle, Key node_handle) {
			TraversalData& nodeTD = getTraversalData(node_handle);

			if(parent_node_handle) {
				TraversalData& parentNodeTD = getTraversalData(parent_node_handle);
				if(!left_node_handle) {
					parentNodeTD.TreeDown = nodeTD.TreeRight;
				}
			}

			if(left_node_handle) {
				TraversalData& leftNodeTD = getTraversalData(left_node_handle);
				leftNodeTD.TreeRight = nodeTD.TreeRight;
			}
		}

		Key mergeNodes(Key node_handle, uint64 new_key) {
			AlphaNode& node = getHelper(node_handle);

			Key parentNodeHandle = node.Parent;

			if(!parentNodeHandle) { return 0; }

			// Get leftmost node with the same key as new key, this is because we transfer all children from nodes with equal keys to the leftmost node

			Key firstSameKeyNodeHandle = 0;

			{
				Key l = getTraversalData(parentNodeHandle).TreeDown;
				while(l) {
					if(getHelper(l).NodeKey == new_key && getTraversalData(l).TypeIndex == getTraversalData(node_handle).TypeIndex) {
						firstSameKeyNodeHandle = l;
						break;
					}

					l = getTraversalData(l).TreeRight;
				}
			}

			{
				Key currentNode = firstSameKeyNodeHandle, lastLowerLevelRightNodeHandle = 0, nextLink = 0, leftMostChildNode = 0, m = 0;

				while(currentNode) { // While there are nodes in this branch, to the right of the first node with the key we are looking for
					TraversalData& td = getTraversalData(currentNode);

					if(getHelper(currentNode).NodeKey != new_key or getTraversalData(currentNode).TypeIndex != getTraversalData(node_handle).TypeIndex or !getHelper(currentNode).Mergeable) { // If node is not mergeable
						if(!nextLink) { // Annotate node which will now be the node to the right of the leftmost node with the same key
							nextLink = currentNode;
						}

						if(td.TreeRight) { //If there is more than one node to our right continue iterating right
							m = currentNode;
							currentNode = td.TreeRight;
						} else {
							m = 0; // Because the last node in this branch isn't of the same key, therefore won't be merged/deleted, we don't have to modify it's previous' nodes (left brother) TreeRight entry.
							break;
						}

						continue;
					}

					// Link the rightmost child from the last mergeable node to the left most node of the current node (which is a mergeable one). This is beacuse all children from all nodes with the same key now have to live under a single parent.
					if(lastLowerLevelRightNodeHandle) {
						getTraversalData(lastLowerLevelRightNodeHandle).TreeRight = td.TreeDown;						
					}

					if(!leftMostChildNode) { // Set leftmost child node which belongs to a node with the key we are looking for.
						leftMostChildNode = td.TreeDown;
					}

					Key currentNodeChild = td.TreeDown;

					// Find rightmost child node to continue hooking up children from different parents
					while(currentNodeChild && getTraversalData(currentNodeChild).TreeRight) {
						currentNodeChild = getTraversalData(currentNodeChild).TreeRight;
					}

					lastLowerLevelRightNodeHandle = currentNodeChild;

					indirectionTable[currentNode - 1] = indirectionTable[firstSameKeyNodeHandle - 1]; // Since we will be "removing nodes" we need to updated their indirection table entries to point to the new node that equates their previous one

					if(currentNode != firstSameKeyNodeHandle) {
						removeNode(parentNodeHandle, m, currentNode);
					}

					if(td.TreeRight) {  //If there is more than one node to our right continue iterating right
						m = currentNode;
						currentNode = td.TreeRight;
					} else {
						break;
					}
				}

				getTraversalData(firstSameKeyNodeHandle).TreeRight = nextLink;
				getTraversalData(firstSameKeyNodeHandle).TreeDown = leftMostChildNode;

				if(m) {
					getTraversalData(m).TreeRight = 0;
				}

				return firstSameKeyNodeHandle;
			}
		}

		TraversalData& getTraversalData(const Key key) {
			return *reinterpret_cast<TraversalData*>(multiTable + indirectionTable[key - 1]);
		}

		[[nodiscard]] const TraversalData& getTraversalData(const Key key) const {
			return *reinterpret_cast<const TraversalData*>(multiTable + indirectionTable[key - 1]);
		}

		byte* getClass(const Key key) const {
			return multiTable + indirectionTable[key - 1] + sizeof(TraversalData);
		}

		AlphaNode& getHelper(Key handle) {
			return alphaTable[handle - 1];
		}

		[[nodiscard]] const AlphaNode& getHelper(Key handle) const {
			return alphaTable[handle - 1];
		}

		inline void tryResizeMultiTable(uint32 delta) {
			if (multiTableSize + delta > multiTableCapacity) {
				if (multiTable) {
					auto copyFunction = [&](uint64 capacity, byte* data, uint64 allocatedElements, byte* newAlloc) {
						for(uint32 i =0, offset = 0; i < entries; ++i) {
							uint8 typeIndex = reinterpret_cast<TraversalData*>(data + offset)->TypeIndex;
							Move(reinterpret_cast<TraversalData*>(data + offset), reinterpret_cast<TraversalData*>(newAlloc + offset)); // Move traversal data block first
							offset += sizeof(TraversalData);
							Move<CLASSES...>(data + offset, newAlloc + offset, typeIndex); // Move with appropriate operator the class element
							offset += GetTypeSize<CLASSES...>(typeIndex);
						}
					};

					ResizeCustom(allocator, &multiTable, &multiTableCapacity, multiTableCapacity * 2, copyFunction);
				} else {
					Allocate(allocator, delta, &multiTable, &multiTableCapacity);
				}
			}
		}

		void tryResizeAlphaTable(uint32 delta) {
			if(entries + delta > alphaCapacity) {
				if(alphaTable) {
					auto ca = alphaCapacity; auto len = entries;

					Resize(allocator, &alphaTable, &ca, ca * 2, entries);
					Resize(allocator, &indirectionTable, &alphaCapacity, ca, entries);
				} else {
					Allocate(allocator, delta, &alphaTable, &alphaCapacity);
					Allocate(allocator, delta, &indirectionTable, &alphaCapacity);
				}
			}
		}

	public:
		friend void ForEachBeta(MultiTree& tree, auto&& a, Key start = 1) {
			if (!tree.GetAlphaLength()) { return; }

			auto visitNode = [&](Key handle, uint32 level, auto&& self) -> void {
				const auto& node = tree.getTraversalData(handle);

				if (node.Way) {
					a(handle, level);

					auto next = node.TreeDown;

					while(next) {
						self(next, level + 1, self);
						next = tree.getTraversalData(next).TreeRight;
					}
				}
			};

			visitNode(start, 0, visitNode);
		}

		friend void ForEach(const MultiTree& tree, auto&& a, auto&& b, uint32 start = 1) {
			if (!tree.GetAlphaLength()) { return; }
			
			auto visitNode = [&](Key handle, uint32 level, auto&& self) -> void {
				const auto& node = tree.getTraversalData(handle);

				if (node.Way) {
					a(handle, level);

					auto next = node.TreeDown;

					while(next) {
						self(next, level + 1, self);
						next = tree.getTraversalData(next).TreeRight;
					}

					b(handle, level);
				}
			};

			visitNode(start, 0, visitNode);
		}

		friend void ForEachWithDisabled(const MultiTree& tree, auto&& a, auto&& b, uint32 start = 1) {
			if (!tree.GetAlphaLength()) { return; }

			auto visitNode = [&](Key handle, uint32 level, auto&& self) -> void {
				const auto& node = tree.getTraversalData(handle);
				
				a(handle, level, node.Way);

				auto next = node.TreeDown;

				while(next) {
					self(next, level + 1, self);
					next = tree.getTraversalData(next).TreeRight;
				}

				b(handle, level, node.Way);
			};

			visitNode(start, 0, visitNode);
		}

		~MultiTree() {
			if (multiTableSize && multiTable) {
				auto visitNode = [&](uint32 key, uint32 level) -> void {
					Destroy<CLASSES...>(getTraversalData(key).TypeIndex, getClass(key));
				};

				ForEach(*this, visitNode, [&](uint32 key, uint32 level) {});

				for (uint32 i = 0; i < entries; ++i) { Destroy(alphaTable[i].Alpha); }

				Deallocate(allocator, multiTableCapacity, multiTable);
				Deallocate(allocator, alphaCapacity, indirectionTable);
				Deallocate(allocator, alphaCapacity, alphaTable);
			}
		}
	};
}
