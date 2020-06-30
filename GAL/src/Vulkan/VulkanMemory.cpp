#include "GAL/Vulkan/VulkanMemory.h"

#include "GAL/Vulkan/Vulkan.h"
#include "GAL/Vulkan/VulkanRenderDevice.h"

GAL::VulkanDeviceMemory::VulkanDeviceMemory(const CreateInfo& createInfo)
{
	VkMemoryAllocateInfo vk_memory_allocate_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	vk_memory_allocate_info.allocationSize = createInfo.Size;
	vk_memory_allocate_info.memoryTypeIndex = createInfo.MemoryType;

	vkAllocateMemory(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_memory_allocate_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &deviceMemory);
}

void GAL::VulkanDeviceMemory::Destroy(RenderDevice* renderDevice)
{
	vkFreeMemory(static_cast<VulkanRenderDevice*>(renderDevice)->GetVkDevice(), deviceMemory, static_cast<VulkanRenderDevice*>(renderDevice)->GetVkAllocationCallbacks());
}

void* GAL::VulkanDeviceMemory::Map(const MapInfo& mapInfo) const
{
	void* data{ 0 };
	vkMapMemory(static_cast<VulkanRenderDevice*>(mapInfo.RenderDevice)->GetVkDevice(), deviceMemory, mapInfo.Offset, mapInfo.Size, 0, &data);
	return data;
}

void GAL::VulkanDeviceMemory::Unmap(const UnmapInfo& unmapInfo) const
{
	vkUnmapMemory(static_cast<VulkanRenderDevice*>(unmapInfo.RenderDevice)->GetVkDevice(), deviceMemory);
}
