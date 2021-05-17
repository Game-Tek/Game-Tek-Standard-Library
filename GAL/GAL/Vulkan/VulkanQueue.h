#pragma once
#include "VulkanCommandBuffer.h"
#include "GTSL/Core.h"

#include "VulkanRenderDevice.h"
#include "VulkanSynchronization.h"
#include "GAL/Queue.h"

namespace GAL {
	class VulkanQueue final : public Queue
	{
	public:
		VulkanQueue() = default;
		~VulkanQueue() = default;

		void Initialize(const VulkanRenderDevice* renderDevice, const VulkanRenderDevice::QueueKey queueKey) {
			familyIndex = queueKey.Family; queueIndex = queueKey.Queue;
			renderDevice->vulkanDLL.LoadDynamicFunction("vkGetDeviceQueue").Call<VkResult>(renderDevice->GetVkDevice(), familyIndex, queueIndex, &queue);
		}

		//void Wait(const class VulkanRenderDevice* renderDevice) const {
		//	vkQueueWaitIdle(queue);
		//}

		void Submit(const VulkanRenderDevice* renderDevice, const GTSL::Range<const WorkUnit*> submitInfos, const VulkanFence fence) {
			VkSubmitInfo vkSubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
			GTSL::Array<VkCommandBuffer, 16> vkCommandBuffers;
			GTSL::Array<VkSemaphore, 16> signalSemaphores;
			GTSL::Array<VkSemaphore, 16> waitSemaphores;
			GTSL::Array<GTSL::uint64, 16> signalValues;
			GTSL::Array<GTSL::uint64, 16> waitValues;
			GTSL::Array<VkPipelineStageFlags, 16> waitPipelineStages;

			for(auto& s : submitInfos) {
				vkCommandBuffers.EmplaceBack(static_cast<const VulkanCommandBuffer*>(s.CommandBuffer)->GetVkCommandBuffer());
				if (s.SignalSemaphore)
					signalSemaphores.EmplaceBack(static_cast<const VulkanSemaphore*>(s.SignalSemaphore)->GetVkSemaphore());
				else
					signalSemaphores.EmplaceBack(static_cast<VkSemaphore>(nullptr));
				waitSemaphores.EmplaceBack(static_cast<const VulkanSemaphore*>(s.WaitSemaphore)->GetVkSemaphore());
				waitPipelineStages.EmplaceBack(ToVulkan(s.WaitPipelineStage));
			}
			
			vkSubmitInfo.commandBufferCount = vkCommandBuffers.GetLength();
			vkSubmitInfo.pCommandBuffers = vkCommandBuffers.begin();
			vkSubmitInfo.waitSemaphoreCount = waitSemaphores.GetLength();
			vkSubmitInfo.pWaitSemaphores = waitSemaphores.begin();
			vkSubmitInfo.signalSemaphoreCount = signalSemaphores.GetLength();
			vkSubmitInfo.pSignalSemaphores = signalSemaphores.begin();
			vkSubmitInfo.pWaitDstStageMask = waitPipelineStages.begin();

			renderDevice->VkQueueSubmit(queue, 1, &vkSubmitInfo, fence.GetVkFence());
		}
			
		void Submit(const VulkanRenderDevice* renderDevice, const GTSL::Range<const GTSL::Range<const WorkUnit*>*> submitInfos, const VulkanFence fence) {
			//VkTimelineSemaphoreSubmitInfo vk_timeline_semaphore_submit_info{ VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO };
			//vk_timeline_semaphore_submit_info.waitSemaphoreValueCount = vk_wait_semaphores.GetLength();
			//vk_timeline_semaphore_submit_info.pWaitSemaphoreValues = submitInfo.WaitValues.begin();
			//vk_timeline_semaphore_submit_info.signalSemaphoreValueCount = vk_signal_semaphores.GetLength();
			//vk_timeline_semaphore_submit_info.pSignalSemaphoreValues = submitInfo.SignalValues.begin();

			GTSL::Array<VkSubmitInfo, 16> vkSubmitInfos;
			GTSL::Array<GTSL::Array<VkCommandBuffer, 16>, 16> commandBuffers;
			GTSL::Array<GTSL::Array<VkSemaphore, 16>, 16> signalSemaphores;
			GTSL::Array<GTSL::Array<VkSemaphore, 16>, 16> waitSemaphores;
			GTSL::Array<GTSL::Array<GTSL::uint64, 16>, 16> signalValues;
			GTSL::Array<GTSL::Array<GTSL::uint64, 16>, 16> waitValues;
			GTSL::Array<GTSL::Array<VkPipelineStageFlags, 16>, 16> waitPipelineStages;
			//vk_submit_info.pNext = &vk_timeline_semaphore_submit_info;

			for (auto& e : submitInfos) {
				auto& vkSubmitInfo = vkSubmitInfos.EmplaceBack();
				auto& vkCommandBuffers = commandBuffers.EmplaceBack();
				auto& vkSignalSemaphores = signalSemaphores.EmplaceBack(); auto& vkWaitSemaphores = waitSemaphores.EmplaceBack();
				auto& sV = signalValues.EmplaceBack(); auto& wV = waitValues.EmplaceBack();
				auto& wPS = waitPipelineStages.EmplaceBack();

				for (auto& wu : e) {
					vkCommandBuffers.EmplaceBack(static_cast<const VulkanCommandBuffer*>(wu.CommandBuffer)->GetVkCommandBuffer());
					vkSignalSemaphores.EmplaceBack(static_cast<const VulkanSemaphore*>(wu.SignalSemaphore)->GetVkSemaphore());
					vkWaitSemaphores.EmplaceBack(static_cast<const VulkanSemaphore*>(wu.SignalSemaphore)->GetVkSemaphore());
					sV.EmplaceBack(wu.SignalValue); wV.EmplaceBack(wu.WaitValue);
					wPS.EmplaceBack(ToVulkan(wu.WaitPipelineStage));
				}

				vkSubmitInfo.commandBufferCount = vkCommandBuffers.GetLength();
				vkSubmitInfo.pCommandBuffers = vkCommandBuffers.begin();

				vkSubmitInfo.pWaitDstStageMask = wPS.begin();

				vkSubmitInfo.signalSemaphoreCount = vkSignalSemaphores.GetLength();
				vkSubmitInfo.pSignalSemaphores = vkSignalSemaphores.begin();

				vkSubmitInfo.waitSemaphoreCount = vkWaitSemaphores.GetLength();
				vkSubmitInfo.pWaitSemaphores = vkWaitSemaphores.begin();
			}

			VK_CHECK(renderDevice->VkQueueSubmit(queue, vkSubmitInfos.GetLength(), vkSubmitInfos.begin(), fence.GetVkFence()))
		}

		[[nodiscard]] VkQueue GetVkQueue() const { return queue; }
		[[nodiscard]] VulkanRenderDevice::QueueKey GetQueueKey() const { return VulkanRenderDevice::QueueKey{ familyIndex, queueIndex }; }

	private:
		VkQueue queue = nullptr;
		GTSL::uint32 queueIndex = 0;
		GTSL::uint32 familyIndex = 0;
	};
}
