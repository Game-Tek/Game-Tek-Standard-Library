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
		VulkanFence(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);
		[[nodiscard]] VkFence GetVkFence() const { return fence; }

		struct WaitForFencesInfo final : RenderInfo
		{
			GTSL::Ranger<const Fence> Fences;
			GTSL::uint64 Timeout;
			bool WaitForAll{ true };
		};
		static void WaitForFences(const WaitForFencesInfo& waitForFencesInfo);

		struct ResetFencesInfo final : RenderInfo
		{
			GTSL::Ranger<const Fence> Fences;
		};
		static void ResetFences(const ResetFencesInfo& resetFencesInfo);
	private:
		VkFence fence{ nullptr };
	};

	class VulkanSemaphore final : public Semaphore
	{
	public:
		VulkanSemaphore() = default;
		VulkanSemaphore(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);
		[[nodiscard]] VkSemaphore GetVkSemaphore() const { return semaphore; }
	private:
		VkSemaphore semaphore{ nullptr };
	};
}
