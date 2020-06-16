#pragma once

#include "RenderCore.h"

namespace GAL
{
	class DeviceMemory : GALObject
	{
	public:
		DeviceMemory() = default;
		
		struct CreateInfo : RenderInfo
		{
			GTSL::uint32 Size{ 0 };
			GTSL::uint32 Alignment{ 0 };

			BufferType BufferUsage;
		};
		DeviceMemory(const CreateInfo& createInfo);
	};
}
