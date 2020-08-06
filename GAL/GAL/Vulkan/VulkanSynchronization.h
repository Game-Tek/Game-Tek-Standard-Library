#pragma once

#include "GAL/Synchronization.h"
#include "Vulkan.h"
#include "GTSL/Ranger.h"

namespace GAL
{
	class VulkanFence final : public Fence
	{
	public:
		VulkanFence() = default;

		struct CreateInfo : VulkanRenderInfo
		{
			bool IsSignaled{ true };
		};
		VulkanFence(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);
		[[nodiscard]] VkFence GetVkFence() const { return fence; }

		[[nodiscard]] bool GetStatus(const VulkanRenderDevice* renderDevice) const;
		
		struct WaitForFencesInfo final : VulkanRenderInfo
		{
			GTSL::Ranger<const VulkanFence> Fences;
			GTSL::uint64 Timeout;
			bool WaitForAll{ true };
		};
		static void WaitForFences(const WaitForFencesInfo& waitForFencesInfo);

		struct ResetFencesInfo final : VulkanRenderInfo
		{
			GTSL::Ranger<const VulkanFence> Fences;
		};
		static void ResetFences(const ResetFencesInfo& resetFencesInfo);
	private:
		VkFence fence{ nullptr };
	};

	class VulkanSemaphore final : public Semaphore
	{
	public:
		VulkanSemaphore() = default;

		struct CreateInfo : VulkanRenderInfo
		{
			GTSL::uint64 InitialValue{ 0 };
		};
		VulkanSemaphore(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);
		[[nodiscard]] VkSemaphore GetVkSemaphore() const { return semaphore; }
	private:
		VkSemaphore semaphore{ nullptr };
	};
}
