#include "GAL/Vulkan/VulkanAccelerationStructures.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

void GAL::VulkanAccelerationStructure::Initialize(const TopLevelCreateInfo& info)
{
	for (auto& e : info.GeometryDescriptors)
	{
		e.stype = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR;
		e.next = nullptr;
	}

	VkAccelerationStructureCreateInfoKHR vkAccelerationStructureCreateInfoKhr{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
	vkAccelerationStructureCreateInfoKhr.flags = info.Flags;
	vkAccelerationStructureCreateInfoKhr.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	vkAccelerationStructureCreateInfoKhr.pGeometryInfos = reinterpret_cast<const VkAccelerationStructureCreateGeometryTypeInfoKHR*>(info.GeometryDescriptors.begin());
	vkAccelerationStructureCreateInfoKhr.maxGeometryCount = info.GeometryDescriptors.ElementCount();
	vkAccelerationStructureCreateInfoKhr.compactedSize = info.CompactedSize;
	vkAccelerationStructureCreateInfoKhr.deviceAddress = info.DeviceAddress;

	info.RenderDevice->vkCreateAccelerationStructureKHR(info.RenderDevice->GetVkDevice(), &vkAccelerationStructureCreateInfoKhr, info.RenderDevice->GetVkAllocationCallbacks(), &accelerationStructure);
	SET_NAME(accelerationStructure, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, info)
}

void GAL::VulkanAccelerationStructure::Initialize(const BottomLevelCreateInfo& info)
{
	for (auto& e : info.GeometryDescriptors)
	{
		e.stype = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR;
		e.next = nullptr;
	}

	VkAccelerationStructureCreateInfoKHR vkAccelerationStructureCreateInfoKhr{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
	vkAccelerationStructureCreateInfoKhr.flags = info.Flags;
	vkAccelerationStructureCreateInfoKhr.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	vkAccelerationStructureCreateInfoKhr.pGeometryInfos = reinterpret_cast<const VkAccelerationStructureCreateGeometryTypeInfoKHR*>(info.GeometryDescriptors.begin());
	vkAccelerationStructureCreateInfoKhr.maxGeometryCount = info.GeometryDescriptors.ElementCount();
	vkAccelerationStructureCreateInfoKhr.compactedSize = info.CompactedSize;
	vkAccelerationStructureCreateInfoKhr.deviceAddress = info.DeviceAddress;

	info.RenderDevice->vkCreateAccelerationStructureKHR(info.RenderDevice->GetVkDevice(), &vkAccelerationStructureCreateInfoKhr, info.RenderDevice->GetVkAllocationCallbacks(), &accelerationStructure);
	SET_NAME(accelerationStructure, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, info)
}

void GAL::VulkanAccelerationStructure::Destroy(const VulkanRenderDevice* renderDevice)
{
	renderDevice->vkDestroyAccelerationStructureKHR(renderDevice->GetVkDevice(), accelerationStructure, renderDevice->GetVkAllocationCallbacks());
	debugClear(accelerationStructure);
}

void GAL::VulkanAccelerationStructure::BindToMemory(const BindToMemoryInfo& info) const
{
	VkBindAccelerationStructureMemoryInfoKHR bind{ VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_KHR };
	bind.accelerationStructure = accelerationStructure;
	bind.deviceIndexCount = 0;
	bind.pDeviceIndices = nullptr;
	bind.memory = static_cast<VkDeviceMemory>(info.Memory.GetVkDeviceMemory());
	bind.memoryOffset = info.Offset;

	info.RenderDevice->vkBindAccelerationStructureMemoryKHR(info.RenderDevice->GetVkDevice(), 1, &bind);
}

GAL::VulkanDeviceAddress GAL::VulkanAccelerationStructure::GetAddress(const VulkanRenderDevice* renderDevice) const
{
	VkAccelerationStructureDeviceAddressInfoKHR deviceAddress{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR };
	deviceAddress.accelerationStructure = accelerationStructure;
	return renderDevice->vkGetAccelerationStructureDeviceAddressKHR(renderDevice->GetVkDevice(), &deviceAddress);
}

void GAL::VulkanAccelerationStructure::BuildAccelerationStructure(const BuildAccelerationStructuresInfo& info)
{
	for(auto& e : info.BuildAccelerationStructureInfos)
	{
		e.stype = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		e.next = nullptr;
	}
	
	info.RenderDevice->vkBuildAccelerationStructureKHR(info.RenderDevice->GetVkDevice(), info.BuildAccelerationStructureInfos.ElementCount(), reinterpret_cast<const VkAccelerationStructureBuildGeometryInfoKHR*>(info.BuildAccelerationStructureInfos.begin()), reinterpret_cast<const VkAccelerationStructureBuildOffsetInfoKHR* const*>(info.BuildOffsets));
}
