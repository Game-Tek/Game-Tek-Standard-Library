#include "GAL/Vulkan/VulkanBuffer.h"

#include "GAL/Vulkan/Vulkan.h"
#include "GAL/Vulkan/VulkanMemory.h"
#include "GAL/Vulkan/VulkanRenderDevice.h"

void GAL::VulkanBuffer::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyBuffer(renderDevice->GetVkDevice(), buffer, renderDevice->GetVkAllocationCallbacks());
	debugClear(buffer);
}

void GAL::VulkanBuffer::GetMemoryRequirements(const VulkanRenderDevice* renderDevice, GTSL::uint32 size,
	VulkanBufferType::value_type bufferType, MemoryRequirements* memoryRequirements)
{
	VkBufferCreateInfo vkBufferCreateInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	vkBufferCreateInfo.size = size;
	vkBufferCreateInfo.usage = bufferType;
	vkBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK(vkCreateBuffer(renderDevice->GetVkDevice(), &vkBufferCreateInfo, renderDevice->GetVkAllocationCallbacks(), &buffer))

	VkMemoryRequirements vkMemoryRequirements;
	vkGetBufferMemoryRequirements(renderDevice->GetVkDevice(), buffer, &vkMemoryRequirements);
	memoryRequirements->Size = static_cast<GTSL::uint32>(vkMemoryRequirements.size);
	memoryRequirements->Alignment = static_cast<GTSL::uint32>(vkMemoryRequirements.alignment);
	memoryRequirements->MemoryTypes = vkMemoryRequirements.memoryTypeBits;
}

void GAL::VulkanBuffer::Initialize(const VulkanRenderDevice* renderDevice, const MemoryRequirements& memoryRequirements, VulkanDeviceMemory memory, GTSL::uint32 offset)
{
	//SET_NAME(buffer, VK_OBJECT_TYPE_BUFFER, info);
	VK_CHECK(vkBindBufferMemory(renderDevice->GetVkDevice(), buffer, static_cast<VkDeviceMemory>(memory.GetVkDeviceMemory()), offset))
}

GAL::VulkanDeviceAddress GAL::VulkanBuffer::GetAddress(const VulkanRenderDevice* renderDevice) const
{
	VkBufferDeviceAddressInfo vk_buffer_device_address_info{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
	vk_buffer_device_address_info.buffer = buffer;
	return vkGetBufferDeviceAddress(renderDevice->GetVkDevice(), &vk_buffer_device_address_info);
}
