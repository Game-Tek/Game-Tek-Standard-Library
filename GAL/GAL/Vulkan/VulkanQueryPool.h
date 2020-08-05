#pragma once

#include "Vulkan.h"
#include "VulkanRenderDevice.h"

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

	inline VulkanQueryPool::VulkanQueryPool(const CreateInfo& createInfo)
	{
		VkQueryPoolCreateInfo vk_query_pool_create_info{ VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO };
		vkCreateQueryPool(createInfo.RenderDevice->GetVkDevice(), &vk_query_pool_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &queryPool);
	}

	inline void VulkanQueryPool::Destroy(VulkanRenderDevice* renderDevice)
	{
		vkDestroyQueryPool(renderDevice->GetVkDevice(), queryPool, renderDevice->GetVkAllocationCallbacks());
	}
}
