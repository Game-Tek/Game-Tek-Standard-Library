#pragma once

#include "GTSL/ArrayCommon.hpp"

namespace GTSL
{
	template<typename T, class ALLOCATOR>
	class SlottedVector
	{
		using FreeSlotsType = Pair<uint32, uint32>;
		
	public:
		void Initialize(const uint32 elements, const ALLOCATOR& allocator)
		{
			this->allocator = allocator; this->allocatedElements = elements; this->freeSlotsTrackerCount = 2;
			this->allocator.Allocate(sizeof(FreeSlotsType) * freeSlotsTrackerCount, alignof(FreeSlotsType), );
			this->allocator.Allocate(sizeof(T) * allocatedElements, alignof(T), );
		}


		template<typename... ARGS>
		Pair<T&, uint32> Emplace(const ARGS&&... args)
		{
			auto result = getFreeIndex(GTSL::Range<FreeSlotsType>(freeSlotsTrackerCount, freeSpaces), &freeSlotsTrackerCount);

			if (!result.State()) { resize(); result = getFreeIndex(GTSL::Range<FreeSlotsType>(freeSlotsTrackerCount, freeSpaces), &freeSlotsTrackerCount); }

			auto* elementLocation = ::new(elements + result.Get()) T(ForwardRef<ARGS>(args)...);
			return Pair<T&, uint32>(*elementLocation, result.Get());
		}
	
	private:
		uint32 freeSlotsTrackerCount, allocatedElements;
		Pair<uint32, uint32>* freeSpaces;
		T* elements;
		[[no_unique_address]] ALLOCATOR allocator;

		void resize()
		{
			
		}
	};
}
