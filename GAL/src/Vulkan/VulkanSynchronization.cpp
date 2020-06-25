#include "GAL/Vulkan/VulkanSynchronization.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

GAL::VulkanFence::VulkanFence(const CreateInfo& createInfo)
{
	VkFenceCreateInfo vk_fence_create_info{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	vk_fence_create_info.flags = createInfo.IsSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	VK_CHECK(vkCreateFence(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_fence_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &fence));
}

void GAL::VulkanFence::Destroy(RenderDevice* renderDevice)
{
	vkDestroyFence(static_cast<VulkanRenderDevice*>(renderDevice)->GetVkDevice(), fence, static_cast<VulkanRenderDevice*>(renderDevice)->GetVkAllocationCallbacks());
}

void GAL::VulkanFence::WaitForFences(const WaitForFencesInfo& waitForFencesInfo)
{
	GTSL::Array<VkFence, 32> vk_fences;
	{
		for (auto& e : waitForFencesInfo.Fences)
		{
			vk_fences.EmplaceBack(static_cast<VulkanFence*>(e)->fence);
		}
	}

	vkWaitForFences(static_cast<VulkanRenderDevice*>(waitForFencesInfo.RenderDevice)->GetVkDevice(), vk_fences.GetLength(), vk_fences.begin(), waitForFencesInfo.WaitForAll, waitForFencesInfo.Timeout);
}

GAL::VulkanSemaphore::VulkanSemaphore(const CreateInfo& createInfo)
{
	VkSemaphoreCreateInfo vk_semaphore_create_info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkSemaphoreTypeCreateInfo vk_semaphore_type_create_info{ VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO };
	vk_semaphore_type_create_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
	vk_semaphore_type_create_info.initialValue = createInfo.InitialValue;
	vk_semaphore_create_info.pNext = &vk_semaphore_type_create_info;

	VK_CHECK(vkCreateSemaphore(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_semaphore_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &semaphore));
}

void GAL::VulkanSemaphore::Destroy(RenderDevice* renderDevice)
{
	vkDestroySemaphore(static_cast<VulkanRenderDevice*>(renderDevice)->GetVkDevice(), semaphore, static_cast<VulkanRenderDevice*>(renderDevice)->GetVkAllocationCallbacks());
}
