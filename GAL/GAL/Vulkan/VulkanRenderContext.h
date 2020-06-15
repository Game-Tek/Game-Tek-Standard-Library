#pragma once

#include "GAL/RenderContext.h"

#include "vulkan/vulkan.h"

#include "VulkanPipelines.h"
#include "VulkanBindings.h"
#include "VulkanRenderTarget.h"

namespace GAL
{
	class VulkanRenderDevice;

	class VulkanRenderContext final : public RenderContext
	{
	public:
		VulkanRenderContext(const CreateInfo& createInfo);
		~VulkanRenderContext() = default;

		void Destroy(RenderDevice* renderDevice);

		void OnResize(const ResizeInfo& _RI);
		void AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo);
		void Flush(const FlushInfo& flushInfo);
		void Present(const PresentInfo& presentInfo);

		struct RenderTargetsInfo : RenderInfo
		{		
		};
		GTSL::Array<GAL::VulkanRenderTarget, 5> GetRenderTargets(const RenderTargetsInfo& renderTargetsInfo);
		
	private:
		VkSurfaceKHR surface = nullptr;
		VkSwapchainKHR swapchain = nullptr;
		VkSurfaceFormatKHR surfaceFormat{};
		VkPresentModeKHR presentMode{};

		GTSL::Array<VkSemaphore, 5, GTSL::uint8> imagesAvailable;
		GTSL::Array<VkSemaphore, 5, GTSL::uint8> rendersFinished;
		GTSL::Array<VkFence, 5, GTSL::uint8> inFlightFences;

		GTSL::uint8 imageIndex = 0;

		VkSurfaceFormatKHR findFormat(const VulkanRenderDevice* device, VkSurfaceKHR surface);
		VkPresentModeKHR findPresentMode(const VkPhysicalDevice _PD, VkSurfaceKHR _Surface);
	};
}
