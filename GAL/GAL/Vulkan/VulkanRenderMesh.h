#pragma once

#include "GAL/RenderMesh.h"

#include "vulkan/vulkan.h"

class VulkanRenderMesh final : public GAL::RenderMesh
{
	VkBuffer buffer = nullptr;
	VkDeviceMemory memory = nullptr;
	size_t indexBufferOffset = 0;

public:
	VulkanRenderMesh(class VulkanRenderDevice* vulkanRenderDevice, const RenderMeshCreateInfo& renderMeshCreateInfo);
	~VulkanRenderMesh() = default;

	void Destroy(class GAL::RenderDevice* renderDevice) override;

	[[nodiscard]] VkBuffer GetVkBuffer() const { return buffer; }
	[[nodiscard]] size_t GetIndexBufferOffset() const { return indexBufferOffset; }
};
