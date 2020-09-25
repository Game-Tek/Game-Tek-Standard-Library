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
			GTSL::Range<const class VulkanTextureView*> TextureViews;
		};
		VulkanFramebuffer(const CreateInfo& info);
		
		~VulkanFramebuffer() = default;

		void Destroy(const class VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkFramebuffer GetVkFramebuffer() const { return framebuffer; }

	private:
		VkFramebuffer framebuffer = nullptr;
	};
}