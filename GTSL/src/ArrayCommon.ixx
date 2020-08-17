module;

#include "GTSL/Memory.h"
#include "GTSL/Core.h"

export module ArrayCommon;

namespace GTSL
{
	export template<typename T>
	void popElement(const Ranger<T> range, uint32 index)
	{
		MemCopy(sizeof(T) * (range.ElementCount() - index - 1), range.begin() + index + 1, range.begin() + index);
	}
}
