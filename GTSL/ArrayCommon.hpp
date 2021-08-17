#pragma once

#include "Result.h"
#include "GTSL/Core.h"
#include "GTSL/Memory.h"
#include "GTSL/Pair.h"

namespace GTSL
{
	template<typename T>
	void popElement(const Range<T*> range, uint32 index)
	{
		++index;
		auto elementCount = (range.end() - range.begin()) - index;
		MemCopy(sizeof(T) * elementCount, range.begin() + index, range.begin() + (index - 1));
	}

	template<typename T>
	void insertElement(const Range<T*> range, uint32 index)
	{
		MemCopy(sizeof(T) * (range.end() - (range.begin() + index)), range.begin() + index, range.begin() + index + 1);
	}

	template<typename  T>
	void copyElementToBack(const Range<T*> range, T* object)
	{
		MemCopy(sizeof(T), object, range.end());
	}

	inline Result<uint32> getFreeIndex(Range<Pair<uint32, uint32>*> freeSpaces, uint32* freeSpacesCount) {
		for(uint32 i = 0; i < *freeSpacesCount; ++i) {
			auto res = freeSpaces[i].First++;
			
			if (freeSpaces[i].First == freeSpaces[i].Second + 1) {
				--(*freeSpacesCount);
				popElement(freeSpaces, i);
			}

			return Result<uint32>(GTSL::MoveRef(res), true);
		}

		return Result<uint32>(false);
	}

	inline void addFreeIndex(uint32 index, GTSL::Range<Pair<uint32, uint32>*> freeSpaces, uint32* freeSpacesCount)
	{
		uint32 lowestIndex = *freeSpacesCount;
		
		for (uint32 i = 0; i < *freeSpacesCount; ++i)
		{
			if(index + 1 == freeSpaces[i].First) { --freeSpaces[i].First; return; }
			if(index + 1 == freeSpaces[i].Second) { ++freeSpaces[i].Second; return; }
			if (index < freeSpaces[i].First) { lowestIndex = i; }
		}

		insertElement(freeSpaces, lowestIndex);
		freeSpaces[lowestIndex].First = index;
		freeSpaces[lowestIndex].Second = index;
	}
}
