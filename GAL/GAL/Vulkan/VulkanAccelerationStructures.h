#pragma once

#define VK_ENABLE_BETA_EXTENSIONS
#include "Vulkan.h"

namespace GAL
{
	class VulkanAccelerationStructure
	{
	public:
		VkAccelerationStructureKHR GetVkAccelerationStructure() const { return accelerationStructure; }
		
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
