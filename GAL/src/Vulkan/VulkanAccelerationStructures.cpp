#include "GAL/Vulkan/VulkanAccelerationStructures.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

GAL::VulkanAccelerationStructure::VulkanAccelerationStructure(const TopLevelCreateInfo& info)
{
	VkAccelerationStructureCreateGeometryTypeInfoKHR acceleration_structure_create_geometry_type{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR };
	acceleration_structure_create_geometry_type.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	acceleration_structure_create_geometry_type.indexType = static_cast<VkIndexType>(info.GeometryType.IndexType);
	acceleration_structure_create_geometry_type.allowsTransforms = info.GeometryType.AllowTransforms;
	acceleration_structure_create_geometry_type.maxPrimitiveCount = info.GeometryType.MaxPrimitiveCount;
	acceleration_structure_create_geometry_type.maxVertexCount = info.GeometryType.MaxVertexCount;
	acceleration_structure_create_geometry_type.vertexFormat = static_cast<VkFormat>(info.GeometryType.VertexType);
	
	VkAccelerationStructureCreateInfoKHR vkAccelerationStructureCreateInfoKhr{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
	vkAccelerationStructureCreateInfoKhr.flags = info.Flags;
	vkAccelerationStructureCreateInfoKhr.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	vkAccelerationStructureCreateInfoKhr.pGeometryInfos = &acceleration_structure_create_geometry_type;
	vkAccelerationStructureCreateInfoKhr.maxGeometryCount = info.MaxGeometryCount;
	vkAccelerationStructureCreateInfoKhr.compactedSize = info.CompactedSize;
	vkAccelerationStructureCreateInfoKhr.deviceAddress = info.DeviceAddress;

	info.RenderDevice->vkCreateAccelerationStructureKHR(info.RenderDevice->GetVkDevice(), &vkAccelerationStructureCreateInfoKhr, info.RenderDevice->GetVkAllocationCallbacks(), &accelerationStructure);
	SET_NAME(accelerationStructure, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, info)
}

GAL::VulkanAccelerationStructure::VulkanAccelerationStructure(const BottomLevelCreateInfo& info)
{
	VkAccelerationStructureCreateGeometryTypeInfoKHR acceleration_structure_create_geometry_type{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR };
	acceleration_structure_create_geometry_type.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	acceleration_structure_create_geometry_type.indexType = static_cast<VkIndexType>(info.GeometryType.IndexType);
	acceleration_structure_create_geometry_type.allowsTransforms = info.GeometryType.AllowTransforms;
	acceleration_structure_create_geometry_type.maxPrimitiveCount = info.GeometryType.MaxPrimitiveCount;
	acceleration_structure_create_geometry_type.maxVertexCount = info.GeometryType.MaxVertexCount;
	acceleration_structure_create_geometry_type.vertexFormat = static_cast<VkFormat>(info.GeometryType.VertexType);

	VkAccelerationStructureCreateInfoKHR vkAccelerationStructureCreateInfoKhr{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
	vkAccelerationStructureCreateInfoKhr.flags = info.Flags;
	vkAccelerationStructureCreateInfoKhr.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	vkAccelerationStructureCreateInfoKhr.pGeometryInfos = &acceleration_structure_create_geometry_type;
	vkAccelerationStructureCreateInfoKhr.maxGeometryCount = info.MaxGeometryCount;
	vkAccelerationStructureCreateInfoKhr.compactedSize = info.CompactedSize;
	vkAccelerationStructureCreateInfoKhr.deviceAddress = info.DeviceAddress;

	info.RenderDevice->vkCreateAccelerationStructureKHR(info.RenderDevice->GetVkDevice(), &vkAccelerationStructureCreateInfoKhr, info.RenderDevice->GetVkAllocationCallbacks(), &accelerationStructure);
	SET_NAME(accelerationStructure, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, info)
}

void GAL::VulkanAccelerationStructure::Destroy(const VulkanRenderDevice* renderDevice)
{
	renderDevice->vkDestroyAccelerationStructureKHR(renderDevice->GetVkDevice(), accelerationStructure, renderDevice->GetVkAllocationCallbacks());
}

void GAL::VulkanAccelerationStructure::BuildAccelerationStructure(const BuildAccelerationStructuresInfo& info)
{
	info.RenderDevice->vkBuildAccelerationStructureKHR(info.RenderDevice->GetVkDevice(), info.BuildAccelerationStructureInfos.ElementCount(), reinterpret_cast<const VkAccelerationStructureBuildGeometryInfoKHR*>(info.BuildAccelerationStructureInfos.begin()), reinterpret_cast<const VkAccelerationStructureBuildOffsetInfoKHR* const*>(info.BuildOffsets));
}
