#include "GAL/Vulkan/VulkanAccelerationStructures.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

GAL::VulkanTopLevelAccelerationStructure::VulkanTopLevelAccelerationStructure(const CreateInfo& createInfo)
{
	VkAccelerationStructureCreateInfoKHR vk_acceleration_structure_create_info_khr{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
	vk_acceleration_structure_create_info_khr.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	
	createInfo.RenderDevice->CreateAccelerationStructure(createInfo.RenderDevice->GetVkDevice(), &vk_acceleration_structure_create_info_khr, createInfo.RenderDevice->GetVkAllocationCallbacks(), &accelerationStructure);
}

void GAL::VulkanTopLevelAccelerationStructure::Destroy(VulkanRenderDevice* renderDevice)
{
	renderDevice->DestroyAccelerationStructure(renderDevice->GetVkDevice(), accelerationStructure, renderDevice->GetVkAllocationCallbacks());
}
