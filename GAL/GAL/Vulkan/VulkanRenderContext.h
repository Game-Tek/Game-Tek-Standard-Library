#pragma once

#include "GAL/RenderContext.h"

#include <GAL/ext/vulkan/vulkan.h>

#include "VulkanPipelines.h"
#include "VulkanBindings.h"
#include "VulkanRenderTarget.h"

namespace GAL
{
	class VulkanRenderContext final : public RenderContext
	{
	public:
		VulkanRenderContext() = default;
		VulkanRenderContext(const CreateInfo& createInfo);
		~VulkanRenderContext() = default;

		void Destroy(class RenderDevice* renderDevice);

		void Recreate(const RecreateInfo& resizeInfo);
		void AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo);
		void Flush(const FlushInfo& flushInfo);
		void Present(const PresentInfo& presentInfo);

		struct RenderTargetsInfo : RenderInfo
		{		
		};
		GTSL::Array<VulkanRenderTarget, 5> GetRenderTargets(const RenderTargetsInfo& renderTargetsInfo);
		
	private:
		VkSurfaceKHR surface = nullptr;
		VkSwapchainKHR swapchain = nullptr;
		VkSurfaceFormatKHR surfaceFormat{};
		VkPresentModeKHR presentMode{};

		GTSL::Array<VkSemaphore, 5, GTSL::uint8> imagesAvailable;
		GTSL::Array<VkSemaphore, 5, GTSL::uint8> rendersFinished;
		GTSL::Array<VkFence, 5, GTSL::uint8> inFlightFences;

		GTSL::uint8 imageIndex = 0;

		VkSurfaceFormatKHR findFormat(class VulkanRenderDevice* device, VkSurfaceKHR surface);
		VkPresentModeKHR findPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	};
}
