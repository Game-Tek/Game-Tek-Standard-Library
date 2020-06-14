#pragma once

#include "RenderCore.h"

#include <GTSL/Extent.h>
#include <GTSL/RGBA.h>
#include <GTSL/FixedVector.hpp>
#include <GTSL/Vector.hpp>

namespace GAL
{
	class RenderPass;
	class RenderTarget;

	struct FramebufferAttachments
	{
		GAL::ImageFormat ColorAttachmentsFormat[8] = {};
		GTSL::uint8 ColorAttachmentsCount = 0;

		GAL::ImageFormat DepthStencilFormat = GAL::ImageFormat::DEPTH16_STENCIL8;

		RenderTarget* Images = nullptr;
	};

	class Framebuffer : public GALObject
	{
	protected:
		GTSL::Extent2D extent;
		GTSL::uint8 attachmentCount = 0;
	public:
		struct CreateInfo : RenderInfo
		{
			RenderPass* RenderPass = nullptr;
			GTSL::Extent2D Extent = { 1280, 720 };
			GTSL::FixedVector<RenderTarget*> Images;
			GTSL::Vector<RGBA> ClearValues;
		};
		explicit Framebuffer(const CreateInfo& createInfo) : extent(createInfo.Extent)
		{
		}

		~Framebuffer() = default;

		[[nodiscard]] GTSL::Extent2D GetExtent() const { return extent; }
		[[nodiscard]] GTSL::uint8 GetAttachmentCount() const { return attachmentCount; };
	};
}
