#pragma once

#include "GAL/RenderContext.h"

#include "vulkan/vulkan.h"

#include "VulkanPipelines.h"
#include "VulkanSwapchainImage.h"
#include "VulkanBindings.h"

#include <GTSL/Vector.hpp>


class VulkanRenderContext final : public GAL::RenderContext
{
	VkSurfaceKHR surface = nullptr;
	VkSwapchainKHR swapchain = nullptr;

	VkSurfaceFormatKHR surfaceFormat{};
	VkPresentModeKHR presentMode{};

	GTSL::Array<VkImage, 5, GTSL::uint8> vulkanSwapchainImages;

	GTSL::Array<VkSemaphore, 5, GTSL::uint8> imagesAvailable;
	GTSL::Array<VkSemaphore, 5, GTSL::uint8> rendersFinished;
	GTSL::Array<VkFence, 5, GTSL::uint8> inFlightFences;
	
	mutable GTSL::Vector<VulkanSwapchainImage> swapchainImages;

	
	GTSL::uint8 imageIndex = 0;

	VkSurfaceFormatKHR FindFormat(const VulkanRenderDevice* device, VkSurfaceKHR surface);
	VkPresentModeKHR FindPresentMode(const VkPhysicalDevice _PD, VkSurfaceKHR _Surface);
public:
	VulkanRenderContext(VulkanRenderDevice* device, const GAL::RenderContextCreateInfo& renderContextCreateInfo);
	~VulkanRenderContext() = default;

	void Destroy(GAL::RenderDevice* renderDevice) override;

	void OnResize(const GAL::ResizeInfo& _RI) override;
	void AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo) override;
	void Flush(const FlushInfo& flushInfo) override;
	void Present(const PresentInfo& presentInfo) override;
};
