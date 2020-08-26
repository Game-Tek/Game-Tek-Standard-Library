#include "GAL/Vulkan/VulkanAsyncHostOperation.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

void GAL::VulkanAsyncHostOperation::Initialize(const CreateInfo& createInfo)
{	
	createInfo.RenderDevice->vkCreateDeferredOperationKHR(createInfo.RenderDevice->GetVkDevice(), createInfo.RenderDevice->GetVkAllocationCallbacks(), reinterpret_cast<VkDeferredOperationKHR*>(&deferredOperation));
}

GTSL::uint32 GAL::VulkanAsyncHostOperation::GetMaxConcurrency(const VulkanRenderDevice* renderDevice)
{
	return renderDevice->vkGetDeferredOperationMaxConcurrencyKHR(renderDevice->GetVkDevice(), static_cast<VkDeferredOperationKHR>(deferredOperation));
}

bool GAL::VulkanAsyncHostOperation::GetResult(const VulkanRenderDevice* renderDevice)
{
	return renderDevice->vkGetDeferredOperationResultKHR(renderDevice->GetVkDevice(), static_cast<VkDeferredOperationKHR>(deferredOperation)) == VK_SUCCESS;
}

void GAL::VulkanAsyncHostOperation::Destroy(const VulkanRenderDevice* renderDevice)
{
	renderDevice->vkDestroyDeferredOperationKHR(renderDevice->GetVkDevice(), static_cast<VkDeferredOperationKHR>(deferredOperation), renderDevice->GetVkAllocationCallbacks());
}

GAL::VulkanAsyncHostOperation::JoinResult GAL::VulkanAsyncHostOperation::Join(const VulkanRenderDevice* renderDevice)
{
	switch (renderDevice->vkDeferredOperationJoinKhr(renderDevice->GetVkDevice(), static_cast<VkDeferredOperationKHR>(deferredOperation)))
	{
	case VK_SUCCESS: return JoinResult::DONE;
	case VK_THREAD_DONE_KHR: return JoinResult::PENDING;
	case VK_THREAD_IDLE_KHR: return JoinResult::WAITING;
	}
}
