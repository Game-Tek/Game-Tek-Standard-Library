#pragma once

#include "GAL/Framebuffer.h"

#include <GAL/ext/vulkan/vulkan.h>

namespace GAL
{
	class VulkanFramebuffer final : public Framebuffer
	{
	public:
		VulkanFramebuffer() = default;
		explicit VulkanFramebuffer(const CreateInfo& createInfo);
		~VulkanFramebuffer() = default;

		void Destroy(RenderDevice* renderDevice) const;

		[[nodiscard]] VkFramebuffer GetVkFramebuffer() const { return framebuffer; }

	private:
		VkFramebuffer framebuffer = nullptr;

	};
}