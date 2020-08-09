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
		GTSL::Array<ImageFormat, 8> ColorAttachmentsFormat;
		ImageFormat DepthStencilFormat = ImageFormat::DEPTH16_STENCIL8;
		class Image* Images = nullptr;
	};

	class Framebuffer : public GALObject
	{
	public:
		Framebuffer() = default;

		~Framebuffer() = default;
	};
}
