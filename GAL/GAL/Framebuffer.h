#pragma once

#include "RenderCore.h"

#include <GTSL/Extent.h>
#include <GTSL/Array.hpp>
#include <GTSL/RGB.h>

namespace GAL
{
	class RenderPass;
	class RenderTarget;

	struct FramebufferAttachments
	{
		GTSL::Array<TextureFormat, 8> ColorAttachmentsFormat;
		TextureFormat DepthStencilFormat = TextureFormat::DEPTH16_STENCIL8;
		class Texture* Images = nullptr;
	};

	class Framebuffer : public GALObject
	{
	public:
		Framebuffer() = default;

		~Framebuffer() = default;
	};
}
