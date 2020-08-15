#pragma once

#define VK_ENABLE_BETA_EXTENSIONS
#include "Vulkan.h"

namespace GAL
{
	class VulkanAccelerationStructure
	{
	public:
		[[nodiscard]] VkAccelerationStructureKHR GetVkAccelerationStructure() const { return accelerationStructure; }

		struct AccelerationStructureGeometryType : VulkanRenderInfo
		{
			VulkanGeometryType GeometryType;
			VulkanIndexType IndexType;
			VulkanShaderDataType VertexType;
			GTSL::uint32 MaxPrimitiveCount = 0;
			GTSL::uint32 MaxVertexCount = 0;
			bool AllowTransforms = false;
		};

		struct AccelerationStructureGeometryTriangleData : VulkanRenderInfo
		{
			VulkanShaderDataType VertexType;
			VulkanIndexType IndexType;
			GTSL::uint16 VertexStride = 0;
			VulkanDeviceAddress VertexBufferAddress;
			VulkanDeviceAddress IndexBufferAddress;
		};

		struct AccelerationStructureGeometry : VulkanRenderInfo
		{
			VulkanGeometryType GeometryType;
			VulkanGeometryFlags GeometryFlags;
			AccelerationStructureGeometryTriangleData* GeometryTriangleData;
		};

		struct AccelerationStructureBuildOffsetInfo : VulkanRenderInfo
		{
			GTSL::uint32 FirstVertex;
			GTSL::uint32 PrimitiveCount;
			GTSL::uint32 PrimitiveOffset;
			GTSL::uint32 TransformOffset;
		};
		
	protected:
		VkAccelerationStructureKHR accelerationStructure;
	};
	
	class VulkanTopLevelAccelerationStructure : public VulkanAccelerationStructure
	{
	public:
		VulkanTopLevelAccelerationStructure() = default;
		struct CreateInfo : VulkanCreateInfo
		{
			
		};
		VulkanTopLevelAccelerationStructure(const CreateInfo& createInfo);
		void Destroy(VulkanRenderDevice* renderDevice);
	private:
	};
}
