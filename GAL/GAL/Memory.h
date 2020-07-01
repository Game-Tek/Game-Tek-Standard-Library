#pragma once

#include "RenderCore.h"

namespace GAL
{
	class DeviceMemory : public GALObject
	{
	public:
		DeviceMemory() = default;
		
		struct CreateInfo : RenderInfo
		{
			GTSL::uint32 Size{ 0 };
			GTSL::uint32 MemoryType{ 0 };
		};
	};
}
