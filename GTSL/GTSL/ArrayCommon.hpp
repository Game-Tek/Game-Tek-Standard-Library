#pragma once

#include "GTSL/Core.h"
#include "GTSL/Memory.h"

namespace GTSL
{
	template<typename T>
	void popElement(const Ranger<T> range, uint32 index)
	{
		MemCopy(sizeof(T) * (range.ElementCount() - index - 1), range.begin() + index + 1, range.begin() + index);
	}

	template<typename T>
	void insertElement(const Ranger<T> range, uint32 index)
	{
		MemCopy(sizeof(T) * (range.ElementCount() - index), range.begin() + index, range.begin() + index + 1);
	}
}
