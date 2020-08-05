#pragma once

#include "Vulkan.h"

namespace GAL
{
	class VulkanQueryPool
	{
	public:
		VulkanQueryPool() = default;

		struct CreateInfo : VulkanRenderInfo
		{
			VulkanQueryType QueryType;
			GTSL::uint32 QueryCount;
		};
		
		VulkanQueryPool(const CreateInfo& createInfo);
		
		void Destroy(VulkanRenderDevice* renderDevice);
		
		[[nodiscard]] VkQueryPool GetVkQueryPool() const { return queryPool; }
	private:
		VkQueryPool queryPool;
	};
}
