#pragma once

#include "GAL/Framebuffer.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanRenderPass;

	class VulkanFramebuffer final : public Framebuffer
	{
	public:
		VulkanFramebuffer() = default;

		struct CreateInfo : VulkanCreateInfo
		{
			VulkanRenderPass* RenderPass = nullptr;
			GTSL::Extent2D Extent = { 1280, 720 };
			GTSL::Ranger<const class VulkanImageView> ImageViews;
			GTSL::Ranger<const GTSL::RGBA> ClearValues;
		};
		explicit VulkanFramebuffer(const CreateInfo& createInfo);
		
		~VulkanFramebuffer() = default;

		void Destroy(const class VulkanRenderDevice* renderDevice) const;

		[[nodiscard]] VkFramebuffer GetVkFramebuffer() const { return framebuffer; }

	private:
		VkFramebuffer framebuffer = nullptr;

	};
}