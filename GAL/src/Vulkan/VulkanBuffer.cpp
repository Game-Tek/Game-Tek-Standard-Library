#include "GAL/Vulkan/VulkanBuffer.h"

#include "GAL/Vulkan/Vulkan.h"
#include "GAL/Vulkan/VulkanMemory.h"
#include "GAL/Vulkan/VulkanRenderDevice.h"

GAL::VulkanBuffer::VulkanBuffer(const CreateInfo& createInfo)
{
	VkBufferCreateInfo vk_buffer_create_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	vk_buffer_create_info.size = createInfo.Size;
	vk_buffer_create_info.usage = createInfo.BufferType;

	VK_CHECK(vkCreateBuffer(createInfo.RenderDevice->GetVkDevice(), &vk_buffer_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &buffer));
	SET_NAME(buffer, VK_OBJECT_TYPE_BUFFER, createInfo);
}

void GAL::VulkanBuffer::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyBuffer(renderDevice->GetVkDevice(), buffer, renderDevice->GetVkAllocationCallbacks());
	debugClear(buffer);
}

void GAL::VulkanBuffer::BindToMemory(const BindMemoryInfo& bindMemoryInfo) const
{
	VK_CHECK(vkBindBufferMemory(bindMemoryInfo.RenderDevice->GetVkDevice(), buffer, static_cast<VkDeviceMemory>(bindMemoryInfo.Memory->GetVkDeviceMemory()), bindMemoryInfo.Offset));
}

GAL::VulkanDeviceAddress GAL::VulkanBuffer::GetAddress(const VulkanRenderDevice* renderDevice) const
{
	VkBufferDeviceAddressInfo vk_buffer_device_address_info{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
	vk_buffer_device_address_info.buffer = buffer;
	return vkGetBufferDeviceAddress(renderDevice->GetVkDevice(), &vk_buffer_device_address_info);
}
