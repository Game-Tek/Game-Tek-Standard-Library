#pragma once

#include "GAL/Synchronization.h"
#include "Vulkan.h"
#include "GTSL/Range.h"

namespace GAL
{
	class VulkanFence final : public Fence
	{
	public:
		VulkanFence() = default;

		struct CreateInfo : VulkanCreateInfo
		{
			bool IsSignaled{ true };
		};
		void Initialize(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);
		[[nodiscard]] VkFence GetVkFence() const { return fence; }

		void Wait(const VulkanRenderDevice* renderDevice) const;
		void Reset(const VulkanRenderDevice* renderDevice) const;
		
		[[nodiscard]] bool GetStatus(const VulkanRenderDevice* renderDevice) const;
		
		struct WaitForFencesInfo final : VulkanRenderInfo
		{
			GTSL::Range<const VulkanFence*> Fences;
			GTSL::uint64 Timeout;
			bool WaitForAll{ true };
		};
		static void WaitForFences(const WaitForFencesInfo& waitForFencesInfo);

		struct ResetFencesInfo final : VulkanRenderInfo
		{
			GTSL::Range<const VulkanFence*> Fences;
		};
		static void ResetFences(const ResetFencesInfo& resetFencesInfo);
	private:
		VkFence fence{ nullptr };
	};

	class VulkanSemaphore final : public Semaphore
	{
	public:
		VulkanSemaphore() = default;

		struct CreateInfo : VulkanCreateInfo
		{
			GTSL::uint64 InitialValue = 0xFFFFFFFFFFFFFFFF;
		};
		void Initialize(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);
		[[nodiscard]] VkSemaphore GetVkSemaphore() const { return semaphore; }
	private:
		VkSemaphore semaphore{ nullptr };
	};

	class VulkanEvent final : public Fence
	{
	public:
		VulkanEvent() = default;
		explicit VulkanEvent(const VulkanRenderDevice* renderDevice);
		explicit VulkanEvent(const VulkanRenderDevice* renderDevice, const GTSL::Range<const GTSL::UTF8*> name);

		void Set(const class VulkanRenderDevice* renderDevice);
		void Reset(const class VulkanRenderDevice* renderDevice);
		
		void Destroy(const class VulkanRenderDevice* renderDevice);
		VkEvent GetVkEvent() const { return event; }

		GTSL::uint64 GetHandle() const { return reinterpret_cast<GTSL::uint64>(event); }
	private:
		VkEvent event = nullptr;
	};
}
