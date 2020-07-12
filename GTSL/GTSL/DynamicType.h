#pragma once

#include "Assert.h"
#include "Id.h"

namespace GTSL
{
#if (_DEBUG)
	class CheckedPointer
	{
	public:
		CheckedPointer(void* data, const char* typeName) : pointer(data), name(typeName)
		{
		}

		operator void* () const { return pointer; }

		template<typename T>
		T* GetAs(const char* tName) const { GTSL_ASSERT(Id64(tName) == name, "Types are not equal, should not be dereferenced!"); return static_cast<T*>(pointer); }
	private:
		void* pointer = nullptr;
		Id64 name;
	};

#define DYNAMIC_TYPE(type, data) CheckedPointer(data, #type)
#define DYNAMIC_CAST(type, dynamicType) dynamicType.GetAs(#type)
#else
#define DYNAMIC_TYPE(type, data) void*
#define DYNAMIC_CAST(type, dynamicType) static_cast<type>(dynamicType>
#endif
}