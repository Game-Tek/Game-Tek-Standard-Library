#pragma once

#include "Core.h"

namespace GTSL
{
	template<typename C>
	class DeferrableContainer
	{
	public:
		DeferrableContainer() = default;

		void Initialize(C&& newContainer)
		{
			container = MoveRef(newContainer);
		}
		
		template<typename... ARGS>
		void EmplaceBack(ARGS&&... args)
		{
			container.EmplaceBack(GTSL::ForwardRef<ARGS>(args)...);
			++processedElements;
		}

		auto operator[](const uint32 i) { return container[i]; }

		void Reset()
		{
			container.Pop(processedElements);
			processedElements -= container.GetLength();
		}
		
	private:
		uint32 processedElements = 0;
		C container;
	};
}