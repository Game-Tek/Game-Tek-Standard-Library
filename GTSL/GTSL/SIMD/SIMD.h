#pragma once

#include "GTSL/Assert.h"
#include "GTSL/Core.h"

namespace GTSL
{
	template<typename T, GTSL::uint64 ALIGNMENT>
	struct AlignedPointer
	{
		explicit AlignedPointer(T* ptr) : pointer(ptr)
		{
			//GTSL_ASSERT(ptr)
			//TODO: CHECK ALIGNMENT
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