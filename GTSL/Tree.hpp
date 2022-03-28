#pragma once
#include "Core.h"
#include "HashMap.hpp"
#include "System.h"
#include "Vector.hpp"
#include "Tuple.hpp"
#include "Algorithm.hpp"

namespace GTSL
{
	template<typename CLASS, std::size_t INDEX>
	bool Destroy(uint32 index, void* p) {
		if (index == INDEX) { Destroy(*static_cast<CLASS*>(p)); return true; }
		return false;
	}

	template<typename... CLASSES, std::size_t... Is>
	void Destroy(uint32 index, void* p, Indices<Is...>) {
		(Destroy<CLASSES, Is>(index, p), ...);
	}

	template<typename... CLASSES>
	void Destroy(uint32 index, void* p) {
		Destroy<CLASSES...>(index, p, BuildIndices<sizeof...(CLASSES)>{});
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

		template<typename C>
		struct Iterator {
			Iterator(C* d, uint32 l, uint32 p, uint32 m) : data(d), level(l), pos(p), i(m) {}

			void operator++() {
				auto& node = data[pos - 1];
				bool down = (bool)node.TreeDown;
				pos = node.TreeRight;
			}

			//bool operator<(const Iterator& other) {
			//	return pos < other.pos;
			//}

			bool operator!=(const Iterator& other) {
				return i != other.i;
			}

			Iterator operator*() { return { data, level, pos, data[pos - 1].ChildrenCount }; }
			operator T&() { return data[pos - 1].Data; }
			operator const T&() const { return data[pos - 1].Data; }

			uint32 GetLevel() const { return level; }
			uint32 GetLength() const { return data[pos - 1].ChildrenCount; }

			[[nodiscard]] auto begin() { return Iterator<C>{ data, level + 1, data[pos - 1].TreeDown, 0 }; }
			[[nodiscard]] auto end() { return Iterator<C>{ data, level + 1, data[pos - 1].TreeDown - 1, data[pos - 1].ChildrenCount }; }

		private:
			C* data;
			uint32 level;
			uint32 pos = 0;
			uint32 i = 0;
		};

		using iterator = Iterator<Node>;
		using const_iterator = Iterator<const Node>;

		iterator begin(const uint32 i = 1) { return Iterator{ nodes, 0, i, 0 }; }
		iterator end(const uint32 i = 1) { return Iterator{ nodes, 0, i - 1, at(i).ChildrenCount }; }

		//[[nodiscard]] const_iterator begin() const { return Iterator{ nodes, 0, 1 }; }
		//[[nodiscard]] const_iterator end() const { return Iterator{ nodes, 0, 0 }; }

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

			Key Parent = 0;
			ALPHA Alpha;
		};

		struct TraversalData {
			uint32 TreeDown = 0, TreeRight = 0;
			uint16 ChildrenCount = 0;
			uint8 TypeIndex = 0;
			bool Way = true;
		};

		MultiTree(const ALLOCATOR& alloc = ALLOCATOR()) : allocator(alloc), outOfOrderNodes(16, allocator) {}

		MultiTree(const uint32 minNodes, const ALLOCATOR& alloc = ALLOCATOR()) : allocator(alloc), outOfOrderNodes(16, allocator) {
			tryResizeAlphaTable(minNodes);
			tryResizeMultiTable(minNodes * 16);
		}

		template<typename T, typename... ARGS>
		Key Emplace(Key leftNodeHandle, Key parentNodeHandle, ARGS&&... params) {
			static_assert(IsInPack<T, CLASSES...>(), "Type T is not in tree's type list.");

			tryResizeAlphaTable(1);

			auto index = entries++;
			auto handleValue = entries;

			::new(alphaTable + index) AlphaNode();
			
			tryResizeMultiTable(sizeof(TraversalData) + sizeof(T));

			indirectionTable[index] = dataTableLength;

			TraversalData& entry = *::new(multiTable + dataTableLength) TraversalData();
			entry.TypeIndex = static_cast<uint8>(GetTypeIndex<T>());

			dataTableLength += sizeof(TraversalData);

			::new(multiTable + dataTableLength) T(GTSL::ForwardRef<ARGS>(params)...);

			dataTableLength += sizeof(T);

			if(parentNodeHandle) {
				getHelper(handleValue).Parent = parentNodeHandle;

				AlphaNode& alphaParent = getHelper(parentNodeHandle);
				TraversalData& parentNodeTraversalData = at(parentNodeHandle);

				if (leftNodeHandle) {
					if(leftNodeHandle == 0xFFFFFFFF) {
						if (parentNodeTraversalData.ChildrenCount) {
							auto handle = parentNodeTraversalData.TreeDown;

							while (auto e = at(handle).TreeRight) {
								handle = e;
							}

							at(handle).TreeRight = handleValue;
						} else {
							parentNodeTraversalData.TreeDown = handleValue;							
						}
					} else {
						auto handle = at(leftNodeHandle).TreeRight;
						at(handleValue).TreeRight = handle;
						at(leftNodeHandle).TreeRight = handleValue;
					}
				} else {
					at(handleValue).TreeRight = parentNodeTraversalData.TreeDown;
					parentNodeTraversalData.TreeDown = handleValue;
				}

				//if(currentNodeHandle != hand + 1) {
				//	outOfOrderNodes.EmplaceBack(currentNodeHandle);
				//}

				parentNodeTraversalData.ChildrenCount += 1;
			}

			return handleValue;
		}

		void ToggleBranch(Key betaNodeBranchTop, bool toggle) {
			TraversalData& branchNode = at(betaNodeBranchTop);
			branchNode.Way = toggle;
		}

		template<typename T>
		static consteval uint32 GetTypeIndex() {
			return GTSL::GetTypeIndex<T, CLASSES...>();
		}

		ALPHA& GetAlpha(const Key i) { return alphaTable[i - 1].Alpha; }
		const ALPHA& GetAlpha(const Key i) const { return alphaTable[i - 1].Alpha; }

		Key GetNodeParent(const Key key) const { return getHelper(key).AlphaParent; }

		uint32 GetChildrenCount(const Key betaNodeHandle) const { return at(betaNodeHandle).ChildrenCount; }
		uint32 GetNodeType(const Key handle) const { return at(handle).TypeIndex; }

		uint32 GetAlphaLength() const { return entries; }

		template<typename T>
		T& GetClass(const Key nodeHandle) {
#if _DEBUG
			if (!(GetTypeIndex<T>() == at(nodeHandle).TypeIndex)) { __debugbreak(); }
#endif
			return *reinterpret_cast<T*>(getClass(nodeHandle));
		}

		template<typename T>
		const T& GetClass(const Key nodeHandle) const {
#if _DEBUG
			if (!(GetTypeIndex<T>() == at(nodeHandle).TypeIndex)) { __debugbreak(); }
#endif
			return *reinterpret_cast<const T*>(multiTable + at(nodeHandle).Position + sizeof(T));
		}

		void Optimize() {
			if (outOfOrderNodes.GetLength()) {
				Vector<Pair<uint32, uint32>, ALLOCATOR> groups;
				SortG(outOfOrderNodes);
				groups.EmplaceBack(1, outOfOrderNodes.front());
				for (uint32 i = 1; i < outOfOrderNodes.GetLength(); ++i) {
					if (outOfOrderNodes[i] == groups.back().Second + 1) { ++groups.back().First; }
					else { groups.EmplaceBack(1, outOfOrderNodes[i]); }
				}
				//do shifts
				outOfOrderNodes.Resize(0);
			}
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
		byte* multiTable = nullptr; uint32 dataTableLength = 0, betaCapacity = 0;

		Vector<Key, ALLOCATOR> outOfOrderNodes;

		TraversalData& at(const Key key) {
			return *reinterpret_cast<TraversalData*>(multiTable + indirectionTable[key - 1]);
		}

		const TraversalData& at(const Key key) const {
			return *reinterpret_cast<const TraversalData*>(multiTable + indirectionTable[key - 1]);
		}

		byte* getClass(const Key key) {
			return multiTable + indirectionTable[key - 1] + sizeof(TraversalData);
		}

		AlphaNode& getHelper(Key handle) {
			return alphaTable[handle - 1];
		}

		[[nodiscard]] const AlphaNode& getHelper(Key handle) const {
			return alphaTable[handle - 1];
		}

		void tryResizeMultiTable(uint32 delta) {
			if (dataTableLength + delta > betaCapacity) {
				if (multiTable) {
					Resize(allocator, &multiTable, &betaCapacity, betaCapacity * 2, dataTableLength);
				} else {
					Allocate(allocator, delta, &multiTable, &betaCapacity);
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

			Key next = start;

			auto visitNode = [&](Key handle, uint32 level, auto&& self) -> void {
				const auto& node = tree.at(handle);

				if (node.Way) {
					a(handle, level);

					next = node.TreeDown ? node.TreeDown : node.TreeRight;

					for (uint32 i = 0; i < node.ChildrenCount; ++i) {
						self(next, level + 1, self);
					}
				}

				next = node.TreeRight;
			};

			visitNode(next, 0, visitNode);			
		}

		friend void ForEach(const MultiTree& tree, auto&& a, auto&& b, uint32 start = 1) {
			if (!tree.GetAlphaLength()) { return; }

			Key next = start;
			
			auto visitNode = [&](Key handle, uint32 level, auto&& self) -> void {
				const auto& node = tree.at(handle);

				if (node.Way) {
					a(handle, level);

					next = node.TreeDown ? node.TreeDown : node.TreeRight;

					for (uint32 i = 0; i < node.ChildrenCount; ++i) {
						self(next, level + 1, self);
					}

					b(handle, level);
				}

				next = node.TreeRight;
			};

			visitNode(next, 0, visitNode);			
		}

		friend void ForEachWithDisabled(const MultiTree& tree, auto&& a, auto&& b, uint32 start = 1) {
			if (!tree.GetAlphaLength()) { return; }

			Key next = start;

			auto visitNode = [&](Key handle, uint32 level, auto&& self) -> void {
				const auto& node = tree.at(handle);
				
				a(handle, level, node.Way);

				next = node.TreeDown ? node.TreeDown : node.TreeRight;

				for (uint32 i = 0; i < node.ChildrenCount; ++i) {
					self(next, level + 1, self);
				}

				b(handle, level, node.Way);

				next = node.TreeRight;
			};

			visitNode(next, 0, visitNode);
		}

		~MultiTree() {
			if (dataTableLength && multiTable) {
				auto visitNode = [&](uint32 key, uint32 level) -> void {
					Destroy<CLASSES...>(at(key).TypeIndex, getClass(key));
				};

				ForEach(*this, visitNode, [&](uint32 key, uint32 level) {});

				for (uint32 i = 0; i < entries; ++i) { Destroy(alphaTable[i].Alpha); }

				Deallocate(allocator, betaCapacity, multiTable);
				Deallocate(allocator, alphaCapacity, indirectionTable);
				Deallocate(allocator, alphaCapacity, alphaTable);
			}
		}
	};
}
