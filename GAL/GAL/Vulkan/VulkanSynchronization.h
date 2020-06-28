#pragma once

#include "GAL/Synchronization.h"
#include "Vulkan.h"

namespace GAL
{
	class VulkanFence final : public Fence
	{
	public:
		VulkanFence() = default;
		VulkanFence(const CreateInfo& createInfo);

		void Destroy(RenderDevice* renderDevice);
		[[nodiscard]] VkFence GetVkFence() const { return fence; }

		struct WaitForFencesInfo : RenderInfo
		{
			GTSL::Ranger<Fence*> Fences;
			GTSL::uint64 Timeout;
			bool WaitForAll{ true };
		};
		static void WaitForFences(const WaitForFencesInfo& waitForFencesInfo);
	private:
		VkFence fence{ nullptr };
	};

	class VulkanSemaphore final : public Semaphore
	{
	public:
		VulkanSemaphore() = default;
		VulkanSemaphore(const CreateInfo& createInfo);

		void Destroy(RenderDevice* renderDevice);
		[[nodiscard]] VkSemaphore GetVkSemaphore() const { return semaphore; }
	private:
		VkSemaphore semaphore{ nullptr };
	};
}