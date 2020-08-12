#pragma once

#include "Vulkan.h"

namespace GAL
{
	class VulkanQueryPool
	{
	public:
		VulkanQueryPool() = default;

		struct CreateInfo : VulkanCreateInfo
		{
			VulkanQueryType QueryType;
			GTSL::uint32 QueryCount;
		};
		
		VulkanQueryPool(const CreateInfo& createInfo);

		struct QueryResultsInfo : VulkanRenderInfo
		{
			void* Data = nullptr;
			GTSL::uint32 Size = 0, QueryCount = 0, Stride = 0;
			bool Wait = true;
		};
		void GetQueryResults(const QueryResultsInfo& info) const;
		
		void Destroy(VulkanRenderDevice* renderDevice);
		
		[[nodiscard]] VkQueryPool GetVkQueryPool() const { return queryPool; }
	private:
		VkQueryPool queryPool;
	};
}
