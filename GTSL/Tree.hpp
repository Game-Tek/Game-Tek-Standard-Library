#pragma once
#include "Core.h"
#include "HashMap.hpp"
#include "System.h"
#include "Vector.hpp"
#include "Tuple.hpp"

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
				parentNode.ChildrenCount += 1;

				Key hand = parentNodeHandle;
				while (at(hand).Next) { hand = at(hand).Next; }

				auto& preLink = at(hand);
				preLink.Next = length;
			} else {
				if (length - 1) {
					Key hand = 1;
					while (at(hand).Next) { hand = at(hand).Next; }
					at(hand).Next = length;
				}
			}

			return length;
		}

		T& operator[](const Key handle) { return at(handle).Data; }
		const T& operator[](const Key handle) const { return at(handle).Data; }

		friend void ForEach(Tree& tree, auto&& a, auto&& b) {
			if (tree.length) {
				Key next = 1;

				auto visitNode = [&](Key handle, auto&& self) -> void {
					a(handle);

					next = tree.at(handle).Next;
					for (uint32 i = 0; i < tree.at(handle).ChildrenCount; ++i) { self(next, self); }

					b(handle);
				};

				visitNode(next, visitNode);
			}
		}

	private:
		struct Node {
			T Data;
			uint32 Next = 0, ChildrenCount = 0;

			template<typename ... ARGS>
			Node(ARGS&&... args) : Data(GTSL::ForwardRef<ARGS>(args)...) {}
		};

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

	template<class ALLOCATOR, typename ALPHA, typename BETA, typename... CLASSES>
	class AlphaBetaTree {
	public:
		using Key = uint32;
		using Types = Tuple<CLASSES...>;

		struct Node {
			//Next: indirection table entry
			uint32 Position = 0, Next = 0, ChildrenCount = 0;
			uint16 TypeIndex = 0;
		};

		struct Helper {
			template<typename... ARGS>
			Helper(ARGS&&... args) : Alpha(ForwardRef<ARGS>(args)... ) {}

			ALPHA Alpha;
			
			StaticMap<uint64, Key, 8> ChildrenMap;

			struct BetaNodeData {
				Key Handle = 0;
				StaticMap<uint64, Key, 8> ChildrenMap;
			};
			StaticVector<BetaNodeData, 8> InternalSiblings;
		};

		AlphaBetaTree(const ALLOCATOR& alloc = ALLOCATOR()) : allocator(alloc) {}

		AlphaBetaTree(const uint32 minNodes, const ALLOCATOR& alloc = ALLOCATOR()) : allocator(alloc) {
			tryResizeIndirectionTable(minNodes);
			tryResizeAlphaTable(minNodes);
			tryResizeBetaTable(minNodes * 16);
		}

		template<typename... ARGS>
		Key EmplaceAlpha(Key parentNodeHandle, ARGS&&... args) {
			tryResizeAlphaTable(1);
			::new(alphaTable + alphaEntries++) Helper(GTSL::ForwardRef<ARGS>(args)...);
			return alphaEntries;
		}

		template<typename T, typename... ARGS>
		Result<Key> EmplaceBeta(uint64 nodeName, Key alphaParentHandle, Key alphaSiblingHandle, ARGS&&... args) {
			static_assert(IsInPack<T, CLASSES...>(), "Type T is not in tree's type list.");

			if constexpr (sizeof...(CLASSES)) {
				tryResizeBetaTable(sizeof(BETA) + sizeof(T));
			} else {
				tryResizeBetaTable(sizeof(BETA));
			}

			tryResizeIndirectionTable(1);

			auto& entry = indirectionTable[indirectionEntries++];
			entry.Position = betaLength;
			entry.Next = 0;
			entry.ChildrenCount = 0;
			entry.TypeIndex = GetTypeIndex<T, CLASSES...>();

			::new(betaTable + betaLength) BETA();
			betaLength += sizeof(BETA);

			if constexpr (sizeof...(CLASSES)) {
				::new(betaTable + betaLength) T(GTSL::ForwardRef<ARGS>(args)...);
				betaLength += sizeof(T);
			}

			Helper& alphaSibling = getHelper(alphaSiblingHandle);
			alphaSibling.ChildrenMap.Emplace(nodeName, indirectionEntries);

			auto& internalSiblingEntry = alphaSibling.InternalSiblings.EmplaceBack();
			internalSiblingEntry.Handle = indirectionEntries;

			if (alphaParentHandle) {
				Helper& alphaParent = getHelper(alphaParentHandle);

				auto r = alphaParent.ChildrenMap.TryEmplace(nodeName, indirectionEntries);
				if (!r) { return Result(static_cast<Key>(r.Get()), true); }

				Key parentNodeHandle = alphaParent.InternalSiblings.back().Handle;
				Node& parentNode = at(parentNodeHandle);

				parentNode.ChildrenCount += 1;

				Key hand = parentNodeHandle;
				while (at(hand).Next) {
					hand = at(hand).Next;
				}
				
				auto& preLink = at(hand);
				preLink.Next = indirectionEntries;

			} else  {
				if (indirectionEntries - 1) {
					Key hand = 1;
					while (at(hand).Next) {
						hand = at(hand).Next;
					}
					at(hand).Next = indirectionEntries;
				}
			}

			return Result(static_cast<Key&&>(indirectionEntries), true);
		}

		template<typename T>
		Result<Key> AddBeta(uint64 nodeName, Key alphaParentNodeHandle, Key alphaSiblingNodeHandle, T val) {
			return EmplaceBeta<T>(nodeName, alphaParentNodeHandle, alphaSiblingNodeHandle, MoveRef(val));
		}

		~AlphaBetaTree() {
			if (betaLength && betaTable) {
				auto visitNode = [&](uint32 handle, auto&& self) -> void {
					auto xx = at(handle).Next;
					for (uint32 i = 0; i < at(handle).ChildrenCount; ++i) {
						self(xx, self);
						xx = at(xx).Next;
					}

					Destroy<CLASSES...>(at(handle).TypeIndex, betaTable + at(handle).Position + sizeof(BETA));
				};

				visitNode(1, visitNode);

				for(uint32 i = 0; i < alphaEntries; ++i) { Destroy(alphaTable[i]); }
			
				Deallocate(allocator, betaCapacity, betaTable);
				Deallocate(allocator, indirectionCapacity, indirectionTable);
				Deallocate(allocator, alphaCapacity, alphaTable);
			}
		}
		
		BETA& GetBeta(const Key betaNodeHandle) {
			return *reinterpret_cast<BETA*>(betaTable + at(betaNodeHandle).Position);
		}

		[[nodiscard]] const BETA& GetBeta(const Key betaNodeHandle) const {
			return *reinterpret_cast<const BETA*>(betaTable + at(betaNodeHandle).Position);
		}

		Key GetBetaHandleFromAlpha(const Key alphaSiblingHandle, uint32 index) const {
			const auto& alphaSibling = getHelper(alphaSiblingHandle);
			index = index >= alphaSibling.InternalSiblings.GetLength() ? alphaSibling.InternalSiblings.GetLength() - 1 : index;
			return alphaSibling.InternalSiblings[index].Handle;
		}

		ALPHA& GetAlpha(const Key i) { return alphaTable[i - 1].Alpha; }
		const ALPHA& GetAlpha(const Key i) const { return alphaTable[i - 1].Alpha; }

		uint32 GetNodeType(const Key handle) const { return at(handle).TypeIndex; }

		uint32 GetAlphaLength() const { return alphaEntries; }
		uint32 GetBetaLength() const { return betaLength; }

		template<typename T>
		T& GetClass(const Key nodeHandle) {
#if _DEBUG
			if (!(GetTypeIndex<T, CLASSES...>() == at(nodeHandle).TypeIndex)) { __debugbreak(); }
#endif
			return *reinterpret_cast<T*>(betaTable + at(nodeHandle).Position + sizeof(BETA));
		}

		template<typename T>
		const T& GetClass(const Key nodeHandle) const {
#if _DEBUG
			if (!(GetTypeIndex<T, CLASSES...>() == at(nodeHandle).TypeIndex)) { __debugbreak(); }
#endif
			return *reinterpret_cast<const T*>(betaTable + at(nodeHandle).Position + sizeof(BETA));
		}

		void Optimize() {
			
		}

	private:
		ALLOCATOR allocator;
		
		byte* betaTable = nullptr; uint32 betaLength = 0, betaCapacity = 0;
		Helper* alphaTable = nullptr; uint32 alphaEntries = 0, alphaCapacity = 0;
		Node* indirectionTable = nullptr; uint32 indirectionEntries = 0, indirectionCapacity = 0;

		Node& at(Key nodeHandle) {
			return indirectionTable[nodeHandle - 1];
		}

		const Node& at(Key nodeHandle) const {
			return indirectionTable[nodeHandle - 1];
		}

		Helper& getHelper(Key handle) {
			return alphaTable[handle - 1];
		}

		const Helper& getHelper(Key handle) const {
			return alphaTable[handle - 1];
		}

		void tryResizeIndirectionTable(uint32 delta) {
			if (indirectionEntries + delta > indirectionCapacity) {
				if (indirectionTable) {
					Resize(allocator, &indirectionTable, &indirectionCapacity, indirectionCapacity * 2, indirectionEntries);
				} else {
					Allocate(allocator, delta, &indirectionTable, &indirectionCapacity);
				}
			}
		}

		void tryResizeBetaTable(uint32 delta) {
			if (betaLength + delta > betaCapacity) {
				if (betaTable) {
					Resize(allocator, &betaTable, &betaCapacity, betaCapacity * 2, betaLength);
				} else {
					Allocate(allocator, delta, &betaTable, &betaCapacity);
				}
			}
		}

		void tryResizeAlphaTable(uint32 delta) {
			if(alphaEntries + delta > alphaCapacity) {
				if(alphaTable) {
					Resize(allocator, &alphaTable, &alphaCapacity, alphaCapacity * 2, alphaEntries);
				} else {
					Allocate(allocator, delta, &alphaTable, &alphaCapacity);
				}
			}
		}

	public:
		friend void ForEachBeta(AlphaBetaTree& tree, auto&& a) {
			if (tree.GetAlphaLength()) {
				auto visitNode = [&](uint32 handle, auto&& self) -> void {
					a(tree.at(handle).Data);

					auto xx = tree.at(handle).Next;
					for (uint32 i = 0; i < tree.at(handle).ChildrenCount; ++i) {
						self(xx, self);
						xx = tree.at(xx).Next;
					}
				};

				visitNode(1, visitNode);
			}
		}

		friend void ForEachBeta(AlphaBetaTree& tree, auto&& a, auto&& b) {
			if (tree.GetAlphaLength()) {
				Key next = 1;

				auto visitNode = [&](Key handle, auto&& self) -> void {
					a(handle);

					next = tree.at(handle).Next;

					for(uint32 i = 0; i < tree.at(handle).ChildrenCount; ++i) {
						self(next, self);
					}

					b(handle);
				};

				visitNode(next, visitNode);
			}
		}
	};
}
