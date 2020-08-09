#include "GAL/Vulkan/VulkanQueryPool.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

GAL::VulkanQueryPool::VulkanQueryPool(const CreateInfo& createInfo)
{
	VkQueryPoolCreateInfo vk_query_pool_create_info{ VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO };
	vk_query_pool_create_info.queryCount = createInfo.QueryCount;
	vk_query_pool_create_info.queryType = static_cast<VkQueryType>(createInfo.QueryType);
	
	VK_CHECK(vkCreateQueryPool(createInfo.RenderDevice->GetVkDevice(), &vk_query_pool_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &queryPool));

	SET_NAME(queryPool, VK_OBJECT_TYPE_QUERY_POOL, createInfo);
}

void GAL::VulkanQueryPool::Destroy(VulkanRenderDevice* renderDevice)
{
	vkDestroyQueryPool(renderDevice->GetVkDevice(), queryPool, renderDevice->GetVkAllocationCallbacks());
}