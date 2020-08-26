#pragma once

#include "Vulkan.h"

namespace GAL
{
	class VulkanRenderDevice;

	class VulkanAsyncHostOperation
	{
	public:
		VulkanAsyncHostOperation() = default;

		struct CreateInfo : VulkanCreateInfo
		{
		};		
		void Initialize(const CreateInfo& createInfo);

		GTSL::uint32 GetMaxConcurrency(const VulkanRenderDevice* renderDevice);
		
		bool GetResult(const VulkanRenderDevice* renderDevice);

		enum class JoinResult
		{
			DONE, PENDING, WAITING
		};
		JoinResult Join(const VulkanRenderDevice* renderDevice);
		void Destroy(const VulkanRenderDevice* renderDevice);

		[[nodiscard]] void* GetVkDeferredHostOperationKHR() const { return deferredOperation; }
		
	private:
		void* deferredOperation;
	};
}
