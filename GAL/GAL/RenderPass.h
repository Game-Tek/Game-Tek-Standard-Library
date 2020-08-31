#pragma once

#include "RenderCore.h"

namespace GAL
{
	constexpr GTSL::uint8 ATTACHMENT_UNUSED = 255;

	class RenderPass : public GALObject
	{
	public:
		RenderPass() = default;
		
		~RenderPass() = default;
	};

}
