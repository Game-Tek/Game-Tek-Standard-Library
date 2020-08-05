#include "GAL/Vulkan/VulkanAccelerationStructures.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

GAL::VulkanTopLevelAccelerationStructure::VulkanTopLevelAccelerationStructure(const CreateInfo& createInfo)
{

	VkAccelerationStructureCreateGeometryTypeInfoKHR acceleration_structure_create_geometry_type{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR };
	acceleration_structure_create_geometry_type.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	acceleration_structure_create_geometry_type.indexType;
	acceleration_structure_create_geometry_type.allowsTransforms;
	acceleration_structure_create_geometry_type.maxPrimitiveCount;
	acceleration_structure_create_geometry_type.maxVertexCount;
	acceleration_structure_create_geometry_type.vertexFormat;

	VkGeometryFlagBitsKHR
	
	VkAccelerationStructureCreateInfoKHR vk_acceleration_structure_create_info_khr{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
	vk_acceleration_structure_create_info_khr.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	//vk_acceleration_structure_create_info_khr.
	
	createInfo.RenderDevice->CreateAccelerationStructure(createInfo.RenderDevice->GetVkDevice(), &vk_acceleration_structure_create_info_khr, createInfo.RenderDevice->GetVkAllocationCallbacks(), &accelerationStructure);
}

void GAL::VulkanTopLevelAccelerationStructure::Destroy(VulkanRenderDevice* renderDevice)
{
	renderDevice->DestroyAccelerationStructure(renderDevice->GetVkDevice(), accelerationStructure, renderDevice->GetVkAllocationCallbacks());
}
