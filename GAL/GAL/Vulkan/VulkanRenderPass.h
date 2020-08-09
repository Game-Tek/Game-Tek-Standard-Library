#pragma once

#include "GAL/RenderPass.h"

#include "GAL/Vulkan/Vulkan.h"

namespace GAL
{
	class VulkanRenderPass final : public RenderPass
	{
	public:
		VulkanRenderPass() = default;

		struct CreateInfo final : VulkanCreateInfo
		{
			RenderPassDescriptor Descriptor;
		};
		explicit VulkanRenderPass(const CreateInfo& createInfo);
		~VulkanRenderPass() = default;

		void Destroy(const class VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkRenderPass GetVkRenderPass() const { return renderPass; }
	private:
		VkRenderPass renderPass = nullptr;
	};
}