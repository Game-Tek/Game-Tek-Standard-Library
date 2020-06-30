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
			GTSL::uint32 Alignment{ 8 };
			GTSL::uint32 BufferType{ 0 };
		};
		explicit Buffer(const CreateInfo& createInfo);

		using BufferPointer = GTSL::uint64;
		
	private:
	};
}
