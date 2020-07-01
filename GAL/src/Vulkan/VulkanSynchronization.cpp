#include "GAL/Vulkan/VulkanSynchronization.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

GAL::VulkanFence::VulkanFence(const CreateInfo& createInfo)
{
	VkFenceCreateInfo vk_fence_create_info{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	vk_fence_create_info.flags = createInfo.IsSignaled;

	VK_CHECK(vkCreateFence(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_fence_create_info,
		static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &fence));
}

void GAL::VulkanFence::Destroy(RenderDevice* renderDevice)
{
	vkDestroyFence(static_cast<VulkanRenderDevice*>(renderDevice)->GetVkDevice(), fence, static_cast<VulkanRenderDevice*>(renderDevice)->GetVkAllocationCallbacks());
}

void GAL::VulkanFence::WaitForFences(const WaitForFencesInfo& waitForFencesInfo)
{
	auto vulkan_fences = GTSL::Ranger<const VulkanFence>(waitForFencesInfo.Fences);

	vkWaitForFences(static_cast<VulkanRenderDevice*>(waitForFencesInfo.RenderDevice)->GetVkDevice(), static_cast<GTSL::uint32>(vulkan_fences.ElementCount()),
	reinterpret_cast<const VkFence*>(vulkan_fences.begin()), waitForFencesInfo.WaitForAll, waitForFencesInfo.Timeout);
}

void GAL::VulkanFence::ResetFences(const ResetFencesInfo& resetFencesInfo)
{
	auto vulkan_fences = GTSL::Ranger<const VulkanFence>(resetFencesInfo.Fences);
	
	vkResetFences(static_cast<VulkanRenderDevice*>(resetFencesInfo.RenderDevice)->GetVkDevice(), vulkan_fences.ElementCount(),
	reinterpret_cast<const VkFence*>(vulkan_fences.begin()));
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

	VK_CHECK(vkCreateSemaphore(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_semaphore_create_info,
		static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &semaphore));
}

void GAL::VulkanSemaphore::Destroy(RenderDevice* renderDevice)
{
	vkDestroySemaphore(static_cast<VulkanRenderDevice*>(renderDevice)->GetVkDevice(), semaphore, static_cast<VulkanRenderDevice*>(renderDevice)->GetVkAllocationCallbacks());
}
