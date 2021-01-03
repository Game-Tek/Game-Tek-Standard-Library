#include "GAL/Vulkan/VulkanAccelerationStructures.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"
#include "GTSL/Array.hpp"

void GAL::VulkanAccelerationStructure::GetMemoryRequirements(GetMemoryRequirementsInfo* memoryRequirements, GTSL::uint32* bufferSize, GTSL::uint32* scratchSize)
{
	GTSL::Array<VkAccelerationStructureGeometryKHR, 8> geometries(memoryRequirements->CreateInfo->GeometryDescriptors.ElementCount());
	buildGeometries(geometries, memoryRequirements->CreateInfo->GeometryDescriptors);

	auto type = memoryRequirements->CreateInfo->IsTopLevel ? VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR : VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	
	VkAccelerationStructureBuildSizesInfoKHR buildSizes{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR };
	VkAccelerationStructureBuildGeometryInfoKHR buildInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR };
	buildInfo.flags = memoryRequirements->Flags;
	buildInfo.type = type;
	buildInfo.geometryCount = geometries.GetLength();
	buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	uint32_t maxPrimitiveCount = 0;

	buildInfo.pGeometries = geometries.begin();

	memoryRequirements->RenderDevice->vkGetAccelerationStructureBuildSizesKHR(memoryRequirements->RenderDevice->GetVkDevice(),
		(VkAccelerationStructureBuildTypeKHR)memoryRequirements->BuildType, &buildInfo, &maxPrimitiveCount, &buildSizes);

	*bufferSize = buildSizes.accelerationStructureSize;
	*scratchSize = buildSizes.buildScratchSize;
}

void GAL::VulkanAccelerationStructure::Initialize(const CreateInfo& info)
{
	VkAccelerationStructureCreateInfoKHR vkAccelerationStructureCreateInfoKhr{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
	vkAccelerationStructureCreateInfoKhr.createFlags = 0;
	vkAccelerationStructureCreateInfoKhr.type = info.IsTopLevel ? VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR : VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	vkAccelerationStructureCreateInfoKhr.offset = 0;
	vkAccelerationStructureCreateInfoKhr.deviceAddress = info.DeviceAddress;
	vkAccelerationStructureCreateInfoKhr.buffer = info.Buffer.GetVkBuffer();
	vkAccelerationStructureCreateInfoKhr.size = info.Size;

	info.RenderDevice->vkCreateAccelerationStructureKHR(info.RenderDevice->GetVkDevice(), &vkAccelerationStructureCreateInfoKhr, info.RenderDevice->GetVkAllocationCallbacks(), &accelerationStructure);
	
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

		GTSL::Array<VkAccelerationStructureGeometryKHR, 8> geometries(source.Geometries.ElementCount());
		buildGeometries(geometries, source.Geometries);

		target.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		target.pNext = nullptr;
		target.flags = source.Flags;
		target.dstAccelerationStructure = source.DestinationAccelerationStructure.GetVkAccelerationStructure();
		target.srcAccelerationStructure = source.SourceAccelerationStructure.GetVkAccelerationStructure();
		target.type = static_cast<VkAccelerationStructureTypeKHR>(source.Type);
		target.pGeometries = geometries.begin();
		target.ppGeometries = nullptr;
		target.geometryCount = geometries.GetLength();
		target.scratchData.deviceAddress = source.ScratchBufferAddress;
		target.mode = source.Update ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR : VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	}

	info.RenderDevice->vkBuildAccelerationStructuresKHR(info.RenderDevice->GetVkDevice(), nullptr, info.BuildAccelerationStructureInfos.ElementCount(), buildGeometryInfo.begin(), reinterpret_cast<const VkAccelerationStructureBuildRangeInfoKHR* const*>(info.BuildOffsets));
}
