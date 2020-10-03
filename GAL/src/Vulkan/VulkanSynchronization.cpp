#include "GAL/Vulkan/VulkanSynchronization.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

GAL::VulkanFence::VulkanFence(const CreateInfo& createInfo)
{
	VkFenceCreateInfo vk_fence_create_info{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	vk_fence_create_info.flags = createInfo.IsSignaled;

	VK_CHECK(vkCreateFence(createInfo.RenderDevice->GetVkDevice(), &vk_fence_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &fence))
	SET_NAME(fence, VK_OBJECT_TYPE_FENCE, createInfo)
}

void GAL::VulkanFence::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyFence(renderDevice->GetVkDevice(), fence, renderDevice->GetVkAllocationCallbacks());
	debugClear(fence);
}

void GAL::VulkanFence::Wait(const VulkanRenderDevice* renderDevice) const
{
	VK_CHECK(vkWaitForFences(renderDevice->GetVkDevice(), 1u, &fence, true, 0xFFFFFFFFFFFFFFFF))
}

void GAL::VulkanFence::Reset(const VulkanRenderDevice* renderDevice) const
{
	VK_CHECK(vkResetFences(renderDevice->GetVkDevice(), 1u, &fence))
}

bool GAL::VulkanFence::GetStatus(const VulkanRenderDevice* renderDevice) const
{
	return vkGetFenceStatus(renderDevice->GetVkDevice(), fence) == VK_SUCCESS;
}

void GAL::VulkanFence::WaitForFences(const WaitForFencesInfo& waitForFencesInfo)
{
	VK_CHECK(vkWaitForFences(waitForFencesInfo.RenderDevice->GetVkDevice(), static_cast<GTSL::uint32>(waitForFencesInfo.Fences.ElementCount()), reinterpret_cast<const VkFence*>(waitForFencesInfo.Fences.begin()), waitForFencesInfo.WaitForAll, waitForFencesInfo.Timeout));
}

void GAL::VulkanFence::ResetFences(const ResetFencesInfo& resetFencesInfo)
{
	VK_CHECK(vkResetFences(resetFencesInfo.RenderDevice->GetVkDevice(), resetFencesInfo.Fences.ElementCount(), reinterpret_cast<const VkFence*>(resetFencesInfo.Fences.begin())))
}

GAL::VulkanSemaphore::VulkanSemaphore(const CreateInfo& createInfo)
{
	//VkSemaphoreCreateInfo vk_semaphore_create_info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	//VkSemaphoreTypeCreateInfo vk_semaphore_type_create_info{ VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO };
	//vk_semaphore_type_create_info.semaphoreType = VK_SEMAPHORE_TYPE_;
	//vk_semaphore_type_create_info.initialValue = createInfo.InitialValue;
	//vk_semaphore_create_info.pNext = &vk_semaphore_type_create_info;

	VkSemaphoreCreateInfo vk_semaphore_create_info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	//VkSemaphoreTypeCreateInfo vk_semaphore_type_create_info{ VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO };

	VK_CHECK(vkCreateSemaphore(createInfo.RenderDevice->GetVkDevice(), &vk_semaphore_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &semaphore))
	SET_NAME(semaphore, VK_OBJECT_TYPE_SEMAPHORE, createInfo)
}

void GAL::VulkanSemaphore::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroySemaphore(renderDevice->GetVkDevice(), semaphore, renderDevice->GetVkAllocationCallbacks());
	debugClear(semaphore);
}
