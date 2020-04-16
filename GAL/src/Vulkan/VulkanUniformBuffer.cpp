#include "GAL/Vulkan/VulkanUniformBuffer.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

VulkanUniformBuffer::VulkanUniformBuffer(VulkanRenderDevice* vulkanRenderDevice, const GAL::UniformBufferCreateInfo& _BCI)
{
	VkBufferCreateInfo vk_buffer_create_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	vk_buffer_create_info.size = _BCI.Size;
	vk_buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	vk_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	vkCreateBuffer(vulkanRenderDevice->GetVkDevice(), &vk_buffer_create_info, vulkanRenderDevice->GetVkAllocationCallbacks(), &buffer);

	VkMemoryRequirements vk_memory_requirements{};

	vkGetBufferMemoryRequirements(vulkanRenderDevice->GetVkDevice(), buffer, &vk_memory_requirements);

	VkMemoryAllocateInfo vk_memory_allocate_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	vk_memory_allocate_info.allocationSize = vk_memory_requirements.size;
	vk_memory_allocate_info.memoryTypeIndex = vulkanRenderDevice->FindMemoryType(vk_memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	vkBindBufferMemory(vulkanRenderDevice->GetVkDevice(), buffer, memory, 0/*offset*/);

	vkMapMemory(vulkanRenderDevice->GetVkDevice(), memory, 0/*offset*/, vk_memory_requirements.size, 0/*flags*/, reinterpret_cast<void**>(&mappedMemoryPointer));

	vulkanRenderDevice->AllocateMemory(&vk_memory_requirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &memory);
}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
}

void VulkanUniformBuffer::Destroy(GAL::RenderDevice* renderDevice)
{
	auto vk_device = static_cast<VulkanRenderDevice*>(renderDevice)->GetVkDevice();
	auto vk_allocation_callbacks = static_cast<VulkanRenderDevice*>(renderDevice)->GetVkAllocationCallbacks();
	vkUnmapMemory(vk_device, memory);
	vkDestroyBuffer(vk_device, buffer, vk_allocation_callbacks);
	vkFreeMemory(vk_device, memory, vk_allocation_callbacks);
}

void VulkanUniformBuffer::UpdateBuffer(const GAL::UniformBufferUpdateInfo& uniformBufferUpdateInfo) const
{
	memcpy(mappedMemoryPointer, static_cast<GTSL::byte*>(uniformBufferUpdateInfo.Data) + uniformBufferUpdateInfo.Offset, uniformBufferUpdateInfo.Size);
}
