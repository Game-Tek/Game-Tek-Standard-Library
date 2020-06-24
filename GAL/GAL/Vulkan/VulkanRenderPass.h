#pragma once

#include "GAL/RenderPass.h"

#include "GAL/Vulkan/Vulkan.h"

namespace GAL
{
	class VulkanRenderPass final : public GAL::RenderPass
	{
	public:
		explicit VulkanRenderPass(const CreateInfo& createInfo);
		~VulkanRenderPass() = default;

		void Destroy(class RenderDevice* renderDevice);

		[[nodiscard]] VkRenderPass GetVkRenderPass() const { return renderPass; }
	private:
		VkRenderPass renderPass = nullptr;
	};
}