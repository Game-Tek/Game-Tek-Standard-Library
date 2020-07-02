#include "GAL/Vulkan/VulkanBuffer.h"

#include "GAL/Vulkan/Vulkan.h"
#include "GAL/Vulkan/VulkanMemory.h"
#include "GAL/Vulkan/VulkanRenderDevice.h"

GAL::VulkanBuffer::VulkanBuffer(const CreateInfo& createInfo)
{
	VkBufferCreateInfo vk_buffer_create_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	vk_buffer_create_info.size = createInfo.Size;
	vk_buffer_create_info.usage = createInfo.BufferType;

	VK_CHECK(vkCreateBuffer(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_buffer_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &buffer))
}

void GAL::VulkanBuffer::Destroy(RenderDevice* renderDevice)
{
	vkDestroyBuffer(static_cast<VulkanRenderDevice*>(renderDevice)->GetVkDevice(), buffer, static_cast<VulkanRenderDevice*>(renderDevice)->GetVkAllocationCallbacks());
}

void GAL::VulkanBuffer::BindToMemory(const BindMemoryInfo& bindMemoryInfo) const
{
	vkBindBufferMemory(static_cast<VulkanRenderDevice*>(bindMemoryInfo.RenderDevice)->GetVkDevice(), buffer,
	static_cast<VkDeviceMemory>(static_cast<VulkanDeviceMemory*>(bindMemoryInfo.Memory)->GetVkDeviceMemory()), bindMemoryInfo.Offset);
}
