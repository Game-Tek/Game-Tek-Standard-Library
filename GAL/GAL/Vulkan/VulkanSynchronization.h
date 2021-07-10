#pragma once

#include "GAL/Synchronization.h"
#include "Vulkan.h"
#include "GTSL/Range.h"
#include "VulkanRenderDevice.h"

namespace GAL
{
	class VulkanFence final : public Fence
	{
	public:
		VulkanFence() = default;
		
		void Initialize(const VulkanRenderDevice* renderDevice, bool isSignaled = false) {
			VkFenceCreateInfo vk_fence_create_info{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
			vk_fence_create_info.flags = isSignaled;

			renderDevice->VkCreateFence(renderDevice->GetVkDevice(), &vk_fence_create_info, renderDevice->GetVkAllocationCallbacks(), &fence);
				//SET_NAME(fence, VK_OBJECT_TYPE_FENCE, createInfo)
		}

		void Destroy(const VulkanRenderDevice* renderDevice) {
			renderDevice->VkDestroyFence(renderDevice->GetVkDevice(), fence, renderDevice->GetVkAllocationCallbacks());
			debugClear(fence);
		}
		
		[[nodiscard]] VkFence GetVkFence() const { return fence; }

		void Wait(const VulkanRenderDevice* renderDevice) const {
			renderDevice->VkWaitForFences(renderDevice->GetVkDevice(), 1u, &fence, true, 0xFFFFFFFFFFFFFFFF);
		}
		void Reset(const VulkanRenderDevice* renderDevice) const {
			renderDevice->VkResetFences(renderDevice->GetVkDevice(), 1u, &fence);
		}
		
		//[[nodiscard]] bool GetStatus(const VulkanRenderDevice* renderDevice) const {
		//	return vkGetFenceStatus(renderDevice->GetVkDevice(), fence) == VK_SUCCESS;
		//}
		
		//struct WaitForFencesInfo final : VulkanRenderInfo
		//{
		//	GTSL::Range<const VulkanFence*> Fences;
		//	GTSL::uint64 Timeout;
		//	bool WaitForAll{ true };
		//};
		//static void WaitForFences(const WaitForFencesInfo& waitForFencesInfo);
		//
		//struct ResetFencesInfo final : VulkanRenderInfo
		//{
		//	GTSL::Range<const VulkanFence*> Fences;
		//};
		//static void ResetFences(const ResetFencesInfo& resetFencesInfo);
	private:
		VkFence fence = nullptr;
	};

	class VulkanSemaphore final : public Semaphore
	{
	public:
		VulkanSemaphore() = default;

		void Initialize(const VulkanRenderDevice* renderDevice, const GTSL::uint64 initialValue = 0xFFFFFFFFFFFFFFFF) {

			VkSemaphoreCreateInfo vkSemaphoreCreateInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
			VkSemaphoreTypeCreateInfo vkSemaphoreTypeCreateInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO };
			vkSemaphoreTypeCreateInfo.semaphoreType = initialValue == 0xFFFFFFFFFFFFFFFF ? VK_SEMAPHORE_TYPE_BINARY : VK_SEMAPHORE_TYPE_TIMELINE;
			vkSemaphoreTypeCreateInfo.initialValue = initialValue == 0xFFFFFFFFFFFFFFFF ? 0 : initialValue;
			vkSemaphoreCreateInfo.pNext = &vkSemaphoreTypeCreateInfo;

			renderDevice->VkCreateSemaphore(renderDevice->GetVkDevice(), &vkSemaphoreCreateInfo, renderDevice->GetVkAllocationCallbacks(), &semaphore);
		}

		void Destroy(const VulkanRenderDevice* renderDevice) {
			renderDevice->VkDestroySemaphore(renderDevice->GetVkDevice(), semaphore, renderDevice->GetVkAllocationCallbacks());
			debugClear(semaphore);
		}
		[[nodiscard]] VkSemaphore GetVkSemaphore() const { return semaphore; }
	private:
		VkSemaphore semaphore{ nullptr };
	};

	class VulkanEvent final : public Fence
	{
	public:
		VulkanEvent() = default;
		
		void Initialize(const VulkanRenderDevice* renderDevice) {
			VkEventCreateInfo vkEventCreateInfo{ VK_STRUCTURE_TYPE_EVENT_CREATE_INFO };
			renderDevice->VkCreateEvent(renderDevice->GetVkDevice(), &vkEventCreateInfo, renderDevice->GetVkAllocationCallbacks(), &event);
		}
		
		void Initialize(const VulkanRenderDevice* renderDevice, const GTSL::Range<const char8_t*> name) {
			VkEventCreateInfo vkEventCreateInfo{ VK_STRUCTURE_TYPE_EVENT_CREATE_INFO };
			renderDevice->VkCreateEvent(renderDevice->GetVkDevice(), &vkEventCreateInfo, renderDevice->GetVkAllocationCallbacks(), &event);

			setName(renderDevice, event, VK_OBJECT_TYPE_EVENT, name);
		}

		void Set(const VulkanRenderDevice* renderDevice) {
			renderDevice->VkSetEvent(renderDevice->GetVkDevice(), event);
		}
		
		void Reset(const VulkanRenderDevice* renderDevice) {
			renderDevice->VkResetEvent(renderDevice->GetVkDevice(), event);
		}
		
		void Destroy(const VulkanRenderDevice* renderDevice) {
			renderDevice->VkDestroyEvent(renderDevice->GetVkDevice(), event, renderDevice->GetVkAllocationCallbacks());
			debugClear(event);
		}
		
		VkEvent GetVkEvent() const { return event; }

		GTSL::uint64 GetHandle() const { return reinterpret_cast<GTSL::uint64>(event); }
	private:
		VkEvent event = nullptr;
	};
}
