#include "GAL/Vulkan/VulkanSynchronization.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

void GAL::VulkanFence::Initialize(const CreateInfo& createInfo)
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

void GAL::VulkanSemaphore::Initialize(const CreateInfo& createInfo)
{
	VkSemaphoreCreateInfo vkSemaphoreCreateInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkSemaphoreTypeCreateInfo vkSemaphoreTypeCreateInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO };
	vkSemaphoreTypeCreateInfo.semaphoreType = createInfo.InitialValue == 0xFFFFFFFFFFFFFFFF ? VK_SEMAPHORE_TYPE_BINARY : VK_SEMAPHORE_TYPE_TIMELINE;
	vkSemaphoreTypeCreateInfo.initialValue = createInfo.InitialValue == 0xFFFFFFFFFFFFFFFF ? 0 : createInfo.InitialValue;
	vkSemaphoreCreateInfo.pNext = &vkSemaphoreTypeCreateInfo;

	VK_CHECK(vkCreateSemaphore(createInfo.RenderDevice->GetVkDevice(), &vkSemaphoreCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &semaphore))
	
	SET_NAME(semaphore, VK_OBJECT_TYPE_SEMAPHORE, createInfo)
}

void GAL::VulkanSemaphore::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroySemaphore(renderDevice->GetVkDevice(), semaphore, renderDevice->GetVkAllocationCallbacks());
	debugClear(semaphore);
}

GAL::VulkanEvent::VulkanEvent(const VulkanRenderDevice* renderDevice)
{
	VkEventCreateInfo vkEventCreateInfo{ VK_STRUCTURE_TYPE_EVENT_CREATE_INFO };
	vkCreateEvent(renderDevice->GetVkDevice(), &vkEventCreateInfo, renderDevice->GetVkAllocationCallbacks(), &event);
}

GAL::VulkanEvent::VulkanEvent(const VulkanRenderDevice* renderDevice, const GTSL::Range<const GTSL::UTF8*> name)
{
	VkEventCreateInfo vkEventCreateInfo{ VK_STRUCTURE_TYPE_EVENT_CREATE_INFO };
	vkCreateEvent(renderDevice->GetVkDevice(), &vkEventCreateInfo, renderDevice->GetVkAllocationCallbacks(), &event);

	VkDebugUtilsObjectNameInfoEXT debug_utils_object_name_info_ext{ VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
	debug_utils_object_name_info_ext.objectHandle = reinterpret_cast<GTSL::uint64>(event);
	debug_utils_object_name_info_ext.objectType = VK_OBJECT_TYPE_EVENT;
	debug_utils_object_name_info_ext.pObjectName = name.begin();
	renderDevice->vkSetDebugUtilsObjectNameEXT(renderDevice->GetVkDevice(), &debug_utils_object_name_info_ext);
}

void GAL::VulkanEvent::Set(const VulkanRenderDevice* renderDevice)
{
	vkSetEvent(renderDevice->GetVkDevice(), event);
}

void GAL::VulkanEvent::Reset(const VulkanRenderDevice* renderDevice)
{
	vkResetEvent(renderDevice->GetVkDevice(), event);
}

void GAL::VulkanEvent::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyEvent(renderDevice->GetVkDevice(), event, renderDevice->GetVkAllocationCallbacks());
	debugClear(event);
}
