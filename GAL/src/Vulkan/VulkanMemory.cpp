#include "GAL/Vulkan/VulkanMemory.h"

#include "GAL/Vulkan/Vulkan.h"
#include "GAL/Vulkan/VulkanRenderDevice.h"

GAL::VulkanDeviceMemory::VulkanDeviceMemory(const CreateInfo& createInfo)
{
	VkMemoryAllocateInfo vk_memory_allocate_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	vk_memory_allocate_info.allocationSize = createInfo.Size;
	vk_memory_allocate_info.memoryTypeIndex = createInfo.MemoryType;

	VK_CHECK(vkAllocateMemory(createInfo.RenderDevice->GetVkDevice(), &vk_memory_allocate_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), reinterpret_cast<::VkDeviceMemory*>(&deviceMemory)));

	SET_NAME(deviceMemory, VK_OBJECT_TYPE_DEVICE_MEMORY, createInfo);
}

void GAL::VulkanDeviceMemory::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkFreeMemory(renderDevice->GetVkDevice(), static_cast<::VkDeviceMemory>(deviceMemory), renderDevice->GetVkAllocationCallbacks());
	debugClear(deviceMemory);
}

void* GAL::VulkanDeviceMemory::Map(const MapInfo& mapInfo) const
{
	void* data{ 0 };
	VK_CHECK(vkMapMemory(mapInfo.RenderDevice->GetVkDevice(), static_cast<::VkDeviceMemory>(deviceMemory), mapInfo.Offset, mapInfo.Size, 0, &data));
	return data;
}

void GAL::VulkanDeviceMemory::Unmap(const UnmapInfo& unmapInfo) const
{
	vkUnmapMemory(static_cast<const VulkanRenderDevice*>(unmapInfo.RenderDevice)->GetVkDevice(), static_cast<::VkDeviceMemory>(deviceMemory));
}
