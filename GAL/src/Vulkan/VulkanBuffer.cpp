#include "GAL/Vulkan/VulkanBuffer.h"

#include "GAL/Vulkan/Vulkan.h"
#include "GAL/Vulkan/VulkanMemory.h"
#include "GAL/Vulkan/VulkanRenderDevice.h"

void GAL::VulkanBuffer::Initialize(const CreateInfo& info)
{
	SET_NAME(buffer, VK_OBJECT_TYPE_BUFFER, info)
	VK_CHECK(vkBindBufferMemory(info.RenderDevice->GetVkDevice(), buffer, static_cast<VkDeviceMemory>(info.Memory.GetVkDeviceMemory()), info.Offset))
}

void GAL::VulkanBuffer::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyBuffer(renderDevice->GetVkDevice(), buffer, renderDevice->GetVkAllocationCallbacks());
	debugClear(buffer);
}

void GAL::VulkanBuffer::GetMemoryRequirements(GetMemoryRequirementsInfo* info)
{
	VkBufferCreateInfo vkBufferCreateInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	vkBufferCreateInfo.size = info->CreateInfo->Size;
	vkBufferCreateInfo.usage = info->CreateInfo->BufferType;
	vkBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK(vkCreateBuffer(info->RenderDevice->GetVkDevice(), &vkBufferCreateInfo, info->RenderDevice->GetVkAllocationCallbacks(), &buffer))
	
	VkMemoryRequirements vkMemoryRequirements;
	vkGetBufferMemoryRequirements(info->RenderDevice->GetVkDevice(), buffer, &vkMemoryRequirements);
	info->MemoryRequirements.Size = static_cast<GTSL::uint32>(vkMemoryRequirements.size);
	info->MemoryRequirements.Alignment = static_cast<GTSL::uint32>(vkMemoryRequirements.alignment);
	info->MemoryRequirements.MemoryTypes = vkMemoryRequirements.memoryTypeBits;
}

GAL::VulkanDeviceAddress GAL::VulkanBuffer::GetAddress(const VulkanRenderDevice* renderDevice) const
{
	VkBufferDeviceAddressInfo vk_buffer_device_address_info{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
	vk_buffer_device_address_info.buffer = buffer;
	return vkGetBufferDeviceAddress(renderDevice->GetVkDevice(), &vk_buffer_device_address_info);
}
