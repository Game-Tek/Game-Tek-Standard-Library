#pragma once

#include "GAL/UniformBuffer.h"

#include "GAL/Vulkan/Vulkan.h"

class VulkanUniformBuffer final : public GAL::UniformBuffer
{
	VkBuffer buffer = nullptr;
	VkDeviceMemory memory = nullptr;

	GTSL::byte* mappedMemoryPointer = nullptr;
public:
	VulkanUniformBuffer(class VulkanRenderDevice* vulkanRenderDevice, const GAL::UniformBufferCreateInfo& _BCI);
	~VulkanUniformBuffer();

	void Destroy(class GAL::RenderDevice* renderDevice) override;

	void UpdateBuffer(const GAL::UniformBufferUpdateInfo& uniformBufferUpdateInfo) const override;

	[[nodiscard]] VkBuffer GetVkBuffer() const { return buffer; }
};
