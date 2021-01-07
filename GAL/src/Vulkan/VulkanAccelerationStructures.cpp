#include "GAL/Vulkan/VulkanAccelerationStructures.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"
#include "GTSL/Array.hpp"

void GAL::VulkanAccelerationStructure::GetMemoryRequirements(const GetMemoryRequirementsInfo& memoryRequirements, GTSL::uint32* accStructureSize, GTSL::uint32* scratchSize)
{
	GTSL::Array<VkAccelerationStructureGeometryKHR, 8> vkGeometries;
	GTSL::Array<uint32_t, 8> primitiveCounts;
	for (auto& e : memoryRequirements.Geometries)
	{
		VkAccelerationStructureGeometryKHR geometryKhr; VkAccelerationStructureBuildRangeInfoKHR buildRange;
		buildGeometryAndRange(e, geometryKhr, buildRange);
		vkGeometries.EmplaceBack(geometryKhr); primitiveCounts.EmplaceBack(e.PrimitiveCount);
	}

	auto type = vkGeometries[0].geometryType == VK_GEOMETRY_TYPE_INSTANCES_KHR ? VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR : VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	
	VkAccelerationStructureBuildGeometryInfoKHR buildInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR };
	buildInfo.flags = memoryRequirements.Flags;
	buildInfo.type = type;
	buildInfo.geometryCount = vkGeometries.GetLength();
	buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	buildInfo.pGeometries = vkGeometries.begin();

	VkAccelerationStructureBuildSizesInfoKHR buildSizes{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR };
	memoryRequirements.RenderDevice->vkGetAccelerationStructureBuildSizesKHR(memoryRequirements.RenderDevice->GetVkDevice(),
		(VkAccelerationStructureBuildTypeKHR)memoryRequirements.BuildType, &buildInfo, primitiveCounts.begin(), &buildSizes);

	*accStructureSize = buildSizes.accelerationStructureSize;
	*scratchSize = buildSizes.buildScratchSize;
}

void GAL::VulkanAccelerationStructure::Initialize(const CreateInfo& info)
{
	VkAccelerationStructureCreateInfoKHR vkAccelerationStructureCreateInfoKhr{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
	vkAccelerationStructureCreateInfoKhr.createFlags = 0;
	vkAccelerationStructureCreateInfoKhr.type = info.Geometries[0].Type == VulkanGeometryType::INSTANCES ? VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR : VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
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
	GTSL::Array<VkAccelerationStructureBuildGeometryInfoKHR, 8> buildGeometryInfos;
	GTSL::Array<GTSL::Array<VkAccelerationStructureGeometryKHR, 8>, 8> geometriesPerAccelerationStructure;
	GTSL::Array<GTSL::Array<VkAccelerationStructureBuildRangeInfoKHR, 8>, 8> buildRangesPerAccelerationStructure;
	GTSL::Array<VkAccelerationStructureBuildRangeInfoKHR*, 8> buildRangesRangePerAccelerationStructure;

	for (GTSL::uint32 accStrInfoIndex = 0; accStrInfoIndex < info.BuildAccelerationStructureInfos.ElementCount(); ++accStrInfoIndex)
	{
		auto& source = info.BuildAccelerationStructureInfos[accStrInfoIndex];

		geometriesPerAccelerationStructure.EmplaceBack();
		buildRangesPerAccelerationStructure.EmplaceBack();
		buildRangesRangePerAccelerationStructure.EmplaceBack(buildRangesPerAccelerationStructure[accStrInfoIndex].begin());

		for (GTSL::uint32 i = 0; i < source.Geometries.ElementCount(); ++i)
		{
			VkAccelerationStructureGeometryKHR accelerationStructureGeometry; VkAccelerationStructureBuildRangeInfoKHR buildRange;
			buildGeometryAndRange(source.Geometries[i], accelerationStructureGeometry, buildRange);
			geometriesPerAccelerationStructure[accStrInfoIndex].EmplaceBack(accelerationStructureGeometry);
			buildRangesPerAccelerationStructure[accStrInfoIndex].EmplaceBack(buildRange);
		}

		VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR };
		buildGeometryInfo.flags = source.Flags;
		buildGeometryInfo.srcAccelerationStructure = source.SourceAccelerationStructure.GetVkAccelerationStructure();
		buildGeometryInfo.dstAccelerationStructure = source.DestinationAccelerationStructure.GetVkAccelerationStructure();
		buildGeometryInfo.type = source.Geometries[0].Type == VulkanGeometryType::INSTANCES ? VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR : VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		buildGeometryInfo.pGeometries = geometriesPerAccelerationStructure[accStrInfoIndex].begin();
		buildGeometryInfo.ppGeometries = nullptr;
		buildGeometryInfo.geometryCount = geometriesPerAccelerationStructure[accStrInfoIndex].GetLength();
		buildGeometryInfo.scratchData.deviceAddress = source.ScratchBufferAddress;
		buildGeometryInfo.mode = source.SourceAccelerationStructure.GetVkAccelerationStructure() ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR : VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		buildGeometryInfos.EmplaceBack(buildGeometryInfo);
	}

	info.RenderDevice->vkBuildAccelerationStructuresKHR(info.RenderDevice->GetVkDevice(), nullptr, info.BuildAccelerationStructureInfos.ElementCount(),
		buildGeometryInfos.begin(), buildRangesRangePerAccelerationStructure.begin());
}
