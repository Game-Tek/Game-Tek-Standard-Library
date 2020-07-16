#pragma once

#include "GTSL/Core.h"
#include "RenderCore.h"

namespace GAL
{
	class Buffer : public GALObject
	{
	public:
		Buffer() = default;
		
		struct CreateInfo final : RenderInfo
		{
			GTSL::uint32 Size{ 0 };
			GTSL::uint32 BufferType{ 0 };
		};
	};
}
