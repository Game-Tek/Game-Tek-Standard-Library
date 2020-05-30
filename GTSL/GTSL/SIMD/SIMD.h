#pragma once

#include "GTSL/Assert.h"
#include "GTSL/Core.h"

namespace GTSL
{
	template<typename T, uint64 ALIGNMENT>
	struct AlignedPointer
	{
		explicit AlignedPointer(T* ptr) : pointer(ptr)
		{
			GTSL_ASSERT(reinterpret_cast<uint64>(ptr) % ALIGNMENT == 0, "Expected aligned pointer is not aligned to boundaries!")
		}

		operator T*() const { return pointer; }

		[[nodiscard]] T* Get() const { return pointer; }
		
	protected:
		T* pointer{nullptr};
	};
	
	template<typename T>
	struct UnalignedPointer
	{
		explicit UnalignedPointer(T* ptr) : pointer(ptr)
		{
		}

		operator T*() const { return pointer; }

		[[nodiscard]] T* Get() const { return pointer; }
		
	protected:
		T* pointer{nullptr};
	};
}