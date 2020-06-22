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
		RenderTarget* Images = nullptr;
	};

	class Framebuffer : public GALObject
	{
	public:
		Framebuffer() = default;
		
		struct CreateInfo : RenderInfo
		{
			RenderPass* RenderPass = nullptr;
			GTSL::Extent2D Extent = { 1280, 720 };
			GTSL::Ranger<RenderTarget*> Images;
			GTSL::Ranger<const GTSL::RGBA> ClearValues;
		};
		explicit Framebuffer(const CreateInfo& createInfo) : extent(createInfo.Extent)
		{
		}

		~Framebuffer() = default;

		[[nodiscard]] GTSL::Extent2D GetExtent() const { return extent; }
		[[nodiscard]] GTSL::uint8 GetAttachmentCount() const { return attachmentCount; };

	protected:
		GTSL::Extent2D extent;
		GTSL::uint8 attachmentCount = 0;
	};
}
