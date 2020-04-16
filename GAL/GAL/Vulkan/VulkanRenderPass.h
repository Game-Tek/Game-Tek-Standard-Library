#pragma once

#include "GAL/RenderPass.h"

#include "GAL/Vulkan/Vulkan.h"

class VulkanRenderPass final : public GAL::RenderPass
{
	VkRenderPass renderPass = nullptr;

public:
	VulkanRenderPass(class VulkanRenderDevice* vulkanRenderDevice, const GAL::RenderPassCreateInfo& renderPassDescriptor);
	~VulkanRenderPass() = default;

	void Destroy(class GAL::RenderDevice* renderDevice) override;

	[[nodiscard]] VkRenderPass GetVkRenderPass() const { return renderPass; }
};
