#pragma once

#include "GTSL/Assert.h"
#include "GTSL/Core.h"

namespace GTSL
{
	template<typename T, GTSL::uint64 ALIGNMENT>
	struct AlignedPointer
	{
	protected:
		T* pointer{nullptr};

	public:
		explicit AlignedPointer(T ptr) : pointer(ptr)
		{
			//GTSL_ASSERT(ptr)
			//TODO: CHECK ALIGNMENT
		}

		operator T*() const { return pointer; }

		[[nodiscard]] T* Get() const { return pointer; }
	};

	template<typename T>
	struct UnalignedPointer
	{
	protected:
		T* pointer{nullptr};

	public:
		explicit UnalignedPointer(T ptr) : pointer(ptr)
		{
			//GTSL_ASSERT(ptr)
			//TODO: CHECK ALIGNMENT
		}

		operator T*() const { return pointer; }

		[[nodiscard]] T* Get() const { return pointer; }
	};
}