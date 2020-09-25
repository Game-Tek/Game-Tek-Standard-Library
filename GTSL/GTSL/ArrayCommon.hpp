#pragma once

#include "GTSL/Core.h"
#include "GTSL/Memory.h"

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
}
