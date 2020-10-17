#include "GAL/Vulkan/VulkanAccelerationStructures.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"
#include "GTSL/Array.hpp"

void GAL::VulkanAccelerationStructure::GetMemoryRequirements(GetMemoryRequirementsInfo* memoryRequirements)
{
	for (auto& e : memoryRequirements->CreateInfo->GeometryDescriptors)
	{
		e.stype = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR;
		e.next = nullptr;
	}

	VkAccelerationStructureCreateInfoKHR vkAccelerationStructureCreateInfoKhr{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
	vkAccelerationStructureCreateInfoKhr.flags = memoryRequirements->CreateInfo->Flags;
	vkAccelerationStructureCreateInfoKhr.type = memoryRequirements->CreateInfo->IsTopLevel ? VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR : VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	vkAccelerationStructureCreateInfoKhr.pGeometryInfos = reinterpret_cast<const VkAccelerationStructureCreateGeometryTypeInfoKHR*>(memoryRequirements->CreateInfo->GeometryDescriptors.begin());
	vkAccelerationStructureCreateInfoKhr.maxGeometryCount = memoryRequirements->CreateInfo->GeometryDescriptors.ElementCount();
	vkAccelerationStructureCreateInfoKhr.compactedSize = memoryRequirements->CreateInfo->CompactedSize;
	vkAccelerationStructureCreateInfoKhr.deviceAddress = memoryRequirements->CreateInfo->DeviceAddress;

	memoryRequirements->RenderDevice->vkCreateAccelerationStructureKHR(memoryRequirements->RenderDevice->GetVkDevice(), &vkAccelerationStructureCreateInfoKhr, memoryRequirements->RenderDevice->GetVkAllocationCallbacks(), &accelerationStructure);
	
	VkAccelerationStructureMemoryRequirementsInfoKHR vkAccelerationStructureMemoryRequirementsInfoKhr{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_KHR };
	vkAccelerationStructureMemoryRequirementsInfoKhr.accelerationStructure = accelerationStructure;
	vkAccelerationStructureMemoryRequirementsInfoKhr.buildType = static_cast<VkAccelerationStructureBuildTypeKHR>(memoryRequirements->BuildType);
	vkAccelerationStructureMemoryRequirementsInfoKhr.type = static_cast<VkAccelerationStructureMemoryRequirementsTypeKHR>(memoryRequirements->MemoryRequirementsType);
	VkMemoryRequirements2 vkMemoryRequirements2{ VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2 };
	memoryRequirements->RenderDevice->vkGetAccelerationStructureMemoryRequirementsKHR(memoryRequirements->RenderDevice->GetVkDevice(), &vkAccelerationStructureMemoryRequirementsInfoKhr, &vkMemoryRequirements2);
	memoryRequirements->MemoryRequirements.Size = vkMemoryRequirements2.memoryRequirements.size;
	memoryRequirements->MemoryRequirements.Alignment = vkMemoryRequirements2.memoryRequirements.alignment;
	memoryRequirements->MemoryRequirements.MemoryTypes = vkMemoryRequirements2.memoryRequirements.memoryTypeBits;
}

void GAL::VulkanAccelerationStructure::Initialize(const CreateInfo& info)
{
	VkBindAccelerationStructureMemoryInfoKHR bind{ VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_KHR };
	bind.accelerationStructure = accelerationStructure;
	bind.deviceIndexCount = 0;
	bind.pDeviceIndices = nullptr;
	bind.memory = static_cast<VkDeviceMemory>(info.Memory.GetVkDeviceMemory());
	bind.memoryOffset = info.Offset;

	info.RenderDevice->vkBindAccelerationStructureMemoryKHR(info.RenderDevice->GetVkDevice(), 1, &bind);
	
	SET_NAME(accelerationStructure, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, info)
}

void GAL::VulkanAccelerationStructure::Destroy(const VulkanRenderDevice* renderDevice)
{
	renderDevice->vkDestroyAccelerationStructureKHR(renderDevice->GetVkDevice(), accelerationStructure, renderDevice->GetVkAllocationCallbacks());
	debugClear(accelerationStructure);
}

GAL::VulkanDeviceAddress GAL::VulkanAccelerationStructure::GetAddress(const VulkanRenderDevice* renderDevice) const
{
	VkAccelerationStructureDeviceAddressInfoKHR deviceAddress{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR };
	deviceAddress.accelerationStructure = accelerationStructure;
	return renderDevice->vkGetAccelerationStructureDeviceAddressKHR(renderDevice->GetVkDevice(), &deviceAddress);
}

void GAL::VulkanAccelerationStructure::BuildAccelerationStructure(const BuildAccelerationStructuresInfo& info)
{
	GTSL::Array<VkAccelerationStructureBuildGeometryInfoKHR, 8> buildGeometryInfo(info.BuildAccelerationStructureInfos.ElementCount());

	for (GTSL::uint32 i = 0; i < info.BuildAccelerationStructureInfos.ElementCount(); ++i)
	{
		auto& source = info.BuildAccelerationStructureInfos[i];
		auto& target = buildGeometryInfo[i];

		target.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		target.pNext = nullptr;
		target.flags = source.Flags;
		target.dstAccelerationStructure = source.DestinationAccelerationStructure.GetVkAccelerationStructure();
		target.srcAccelerationStructure = source.SourceAccelerationStructure.GetVkAccelerationStructure();
		target.type = static_cast<VkAccelerationStructureTypeKHR>(source.Type);
		target.geometryArrayOfPointers = source.IsArrayOfPointers;
		target.geometryCount = static_cast<GTSL::uint32>(source.Geometries.ElementCount());
		target.scratchData.deviceAddress = source.ScratchBufferAddress;
		target.update = source.Update;
	}
	
	info.RenderDevice->vkBuildAccelerationStructureKHR(info.RenderDevice->GetVkDevice(), info.BuildAccelerationStructureInfos.ElementCount(), buildGeometryInfo.begin(), reinterpret_cast<const VkAccelerationStructureBuildOffsetInfoKHR* const*>(info.BuildOffsets));
}
