#pragma once

#define VK_ENABLE_BETA_EXTENSIONS
#include "Vulkan.h"

namespace GAL
{
	class VulkanAccelerationStructure
	{
	public:
		VkAccelerationStructureKHR GetVkAccelerationStructure() const { return accelerationStructure; }

		struct AccelerationStructureGeometry : VulkanRenderInfo
		{
			VulkanGeometryType GeometryType;
			VulkanIndexType IndexType;
			VulkanShaderDataType VertexType;
			GTSL::uint32 MaxPrimitiveCount = 0;
			GTSL::uint32 MaxVertexCount = 0;
			bool AllowTransforms = false;
		};
		
	protected:
		VkAccelerationStructureKHR accelerationStructure;
	};
	
	class VulkanTopLevelAccelerationStructure : public VulkanAccelerationStructure
	{
	public:
		VulkanTopLevelAccelerationStructure() = default;
		struct CreateInfo : VulkanRenderInfo
		{
			
		};
		VulkanTopLevelAccelerationStructure(const CreateInfo& createInfo);
		void Destroy(VulkanRenderDevice* renderDevice);
	private:
	};
}
