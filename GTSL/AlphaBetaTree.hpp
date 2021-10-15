#pragma once
#include "Core.h"
#include "System.h"
#include "Vector.hpp"
#include "Tuple.h"

namespace GTSL
{
	template<typename Target, typename ListHead, typename... ListTails>
	constexpr uint32 GetTypeIndexInTemplateList() {
		if constexpr (std::is_same_v<Target, ListHead>)
			return 0u;
		else
			return 1u + GetTypeIndexInTemplateList<Target, ListTails...>();
	}

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

	template<class ALLOCATOR, typename ALPHA, typename... CLASSES>
	class AlphaBetaTree {
	public:
		using Key = uint32;
		using Types = Tuple<CLASSES...>;

		struct Node {
			//Next: indirection table entry
			uint32 Position = 0, Next = 0, ChildrenCount = 0;
			uint16 TypeIndex = 0;
		};

		AlphaBetaTree(const ALLOCATOR& alloc = ALLOCATOR()) : allocator(alloc) {}

		AlphaBetaTree(const uint32 minNodes, const ALLOCATOR& alloc = ALLOCATOR()) : allocator(alloc) {
			tryResizeIndirectionTable(minNodes);
			tryResizeAlphaTable(minNodes);
			tryResizeBetaTable(minNodes * 8);
		}

		Key EmplaceAlpha(Key parentNodeHandle, ALPHA alpha) {
			tryResizeAlphaTable(1);
			alphaTable[alphaEntries++] = alpha;
			return alphaEntries;
		}

		//template<typename T, typename... ARGS>
		//Key AddBeta(uint32 parentNodeHandle, ARGS&&... args) {
		template<typename T, typename... ARGS>
		Key EmplaceBeta(uint32 parentNodeHandle, ARGS&&... args) {
			tryResizeBetaTable(sizeof(T));
			tryResizeIndirectionTable(1);

			auto& entry = indirectionTable[indirectionEntries++];
			entry.Position = betaLength;
			entry.Next = 0;
			entry.ChildrenCount = 0;
			entry.TypeIndex = GetTypeIndexInTemplateList<T, CLASSES...>();

			::new(betaTable + betaLength) T(GTSL::ForwardRef<ARGS>(args)...);
			betaLength += sizeof(T);

			if (parentNodeHandle) {
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

			return indirectionEntries;
		}

		template<typename T>
		Key AddBeta(uint32 parentNodeHandle, T val) {
			return EmplaceBeta<T>(parentNodeHandle, MoveRef(val));
		}

		~AlphaBetaTree() {
			if (betaLength && betaTable) {
				auto visitNode = [&](uint32 handle, auto&& self) -> void {
					auto xx = at(handle).Next;
					for (uint32 i = 0; i < at(handle).ChildrenCount; ++i) {
						self(xx, self);
						xx = at(xx).Next;
					}

					Destroy<CLASSES...>(at(handle).TypeIndex, betaTable + at(handle).Position);
				};

				visitNode(1, visitNode);

				for(uint32 i = 0; i < alphaEntries; ++i) { Destroy(alphaTable[i]); }
			
				Deallocate(allocator, betaTable, betaCapacity);
				Deallocate(allocator, indirectionTable, indirectionCapacity);
				Deallocate(allocator, alphaTable, alphaCapacity);
			}
		}

		template<typename T>
		T& At(const uint32 i) { return *reinterpret_cast<T*>(betaTable + indirectionTable[i - 1].Position); }

		ALPHA& At(const Key i) { return alphaTable[i - 1]; }

		uint32 GetNodeType(const Key handle) { return at(handle).TypeIndex; }

		uint32 GetAlphaLength() const { return alphaEntries; }
		uint32 GetBetaLength() const { return betaLength; }

	private:
		ALLOCATOR allocator;
		
		byte* betaTable = nullptr; uint32 betaLength = 0, betaCapacity = 0;
		ALPHA* alphaTable = nullptr; uint32 alphaEntries = 0, alphaCapacity = 0;
		Node* indirectionTable = nullptr; uint32 indirectionEntries = 0, indirectionCapacity = 0;

		Node& at(uint32 nodeHandle) {
			return indirectionTable[nodeHandle - 1];
		}

		void tryResizeIndirectionTable(uint32 delta) {
			if (indirectionEntries + delta > indirectionCapacity) {
				if (indirectionTable) {
					auto copy = [len = indirectionEntries](const uint64 capacity, Node* nodes, const uint64 newCapacity, Node* newAlloc) {
						Copy(len, nodes, newAlloc);
					};

					Resize(allocator, &indirectionTable, &indirectionCapacity, indirectionCapacity * 2, copy);
				} else {
					Allocate(allocator, delta, &indirectionTable, &indirectionCapacity);
				}
			}
		}

		void tryResizeBetaTable(uint32 delta) {
			if (betaLength + delta > betaCapacity) {
				if (betaTable) {
					auto copy = [len = betaLength](const uint64 capacity, byte* nodes, const uint64 newCapacity, byte* newAlloc) {
						Copy(len, nodes, newAlloc);
					};
					Resize(allocator, &betaTable, &betaCapacity, betaCapacity * 2, copy);
				} else {
					Allocate(allocator, delta, &betaTable, &betaCapacity);
				}
			}
		}

		void tryResizeAlphaTable(uint32 delta) {
			if(alphaEntries + delta > alphaCapacity) {
				if(alphaTable) {
					auto copy = [len = alphaEntries](const uint64 capacity, ALPHA* nodes, const uint64 newCapacity, ALPHA* newAlloc) {
						Copy(len, nodes, newAlloc);
					};
					Resize(allocator, &alphaTable, &alphaCapacity, alphaCapacity * 2, copy);
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
