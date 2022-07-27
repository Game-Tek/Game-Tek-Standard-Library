#pragma once

#include "GTSL/Core.h"
#include "GTSL/ArrayCommon.hpp"

namespace GTSL {

	//
	//   Source Range
	//
	//   Start   Count
	// |-------|-------| |-------|-------| |-------|-------| |-------|-------|
	// |   2   |   2   | |   6   |   1   | |   2   |   2   | |   2   |   2   |
	// |-------|-------| |-------|-------| |-------|-------| |-------|-------|
	// 
	//
	// |---------------------------------|
	// |             0 -> 2              |
	// |---------------------------------|
	//
	// Output Range

	template<class ALLOCATOR>
	class IndirectionMap {
	public:
		~IndirectionMap() {
			if(table)
				Deallocate(allocator, capacity, table);
		}

		void Insert(uint32 key) {
			uint32 i = 0;

			while(i < length && key > table[i].SourceRangeStart + table[i].ContinuousElementsCount) {
				++i;
			}

			if(length) {
				GTSL_ASSERT(!isValueInSourceRange(key, table[i]), "");

				if(key < table[i].SourceRangeStart) { // If key is before current entry
					if(key == table[i].SourceRangeStart - 1) { // Key is continuous but one position before
						--table[i].SourceRangeStart;
						auto f = std::ranges::find(free, table[i].OutputRangeStart - 1);
						if(f) {
							--table[i].OutputRangeStart;
							extendFreeSpace(f - free);
						} else {
							GTSL_ASSERT(false, "");
						}
					} else { // Key is not contiguous so we insert a new entry
						if(length + 1 > capacity) {						
							AllocateOrResize(allocator, &table, &capacity, capacity * 2, length);
						}

						InsertElement(capacity, &length, table, i, key, 0, free[freeSpaces - 1]);

						extendFreeSpace();
					}
				} else { // Key is greater than start value
					if(key > table[i].SourceRangeStart + table[i].ContinuousElementsCount) {
						if(key > table[i].SourceRangeStart + table[i].ContinuousElementsCount + 1) {
							if(length + 1 > capacity) {						
								AllocateOrResize(allocator, &table, &capacity, capacity * 2, length);
							}

							InsertElement(capacity, &length, table, i, key, 0, free[freeSpaces - 1]);
							extendFreeSpace();
						} // Else new key is bigger by only one, so just up ContinuousElementCount
					}
				}
			} else { // If no entries
				AllocateOrResize(allocator, &table, &capacity, 4, length);
				EmplaceBack(&length, table, key, 0, free[freeSpaces - 1]);
				extendFreeSpace();
			}

			table[i].ContinuousElementsCount += 1;
		}

		void Remove(uint32 key) {
			uint32 i = 0;

			while(i < length && key > (table[i].SourceRangeStart + table[i].ContinuousElementsCount)) {
				++i;
			}

			if(!--table[i].ContinuousElementsCount) {
				insertFreeSpace(table[i].OutputRangeStart);
				PopElement(capacity, &length, table, i);
				return; // If no elements are left in entry after removing this one, then entry is empty and must be removed, therefore no further action must be performed on this entry and we return
			}

			if(key > table[i].SourceRangeStart) { // Key value was somewhere in the middle of the entries range so we must split entry to keep it valid since the range is no longer continuous
				insertFreeSpace(table[i].OutputRangeStart + key - table[i].SourceRangeStart);
				Insert(key + 1);
				return;
			}

			if(key == table[i].SourceRangeStart) { // Key is entry start value, so just increment start 
				insertFreeSpace(table[i].OutputRangeStart);
				++table[i].SourceRangeStart;
				++table[i].OutputRangeStart;
			}
		}

		uint32 operator[](const uint32 key) const {
			uint32 i = 0;

			while(i < length && key > (table[i].SourceRangeStart + table[i].ContinuousElementsCount)) {
				++i;
			}

			return table[i].OutputRangeStart + key - table[i].SourceRangeStart;
		}

		void extendFreeSpace(const uint32 pos){
			free[pos] += 1;
			uses[pos] -= 1;

			if(!uses[freeSpaces - 1]) {
				--freeSpaces;
			}
		}

		void extendFreeSpace(){
			extendFreeSpace(0);
		}

		void insertFreeSpace(const uint32 value) {
			free[freeSpaces] = value;
			uses[freeSpaces] = 1;
			++freeSpaces;
		}

	private:
		ALLOCATOR allocator;

		uint32 length = 0, capacity = 0;
		uint32 minValue = 0, maxValue = 0;

		struct Entry {
			uint32 SourceRangeStart = 0, ContinuousElementsCount = 0, OutputRangeStart = 0;
		};

		Entry* table = nullptr;

		uint32 free[32] { 0 };
		uint32 uses[32] { 0xFFFFFFFF };
		uint32 freeSpaces = 1;

		static bool isValueInSourceRange(const uint32 key, Entry entry) {
			return key >= entry.SourceRangeStart && key < entry.SourceRangeStart + entry.ContinuousElementsCount;
		}

		static bool isValueInOutputRange(const uint32 key, Entry entry) {
			return key >= entry.OutputRangeStart && key < entry.OutputRangeStart + entry.ContinuousElementsCount;
		}
	};
}