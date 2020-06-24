#pragma once

#include "GAL/RenderContext.h"

#include <GAL/ext/vulkan/vulkan.h>

#include "VulkanPipelines.h"
#include "VulkanBindings.h"
#include "VulkanImage.h"

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
		/**
		 * \brief  Acquires the next image in the swapchain queue to present to.
		 * \param acquireNextImageInfo Information to perform image acquisition.
		 * \return Returns true if the contexts needs to be recreated.
		 */
		bool AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo);
		void Present(const PresentInfo& presentInfo);

		[[nodiscard]] GTSL::uint8 GetCurrentImage() const { return imageIndex; }
		
		struct GetImagesInfo : RenderInfo
		{		
		};
		GTSL::Array<VulkanImage, 5> GetImages(const GetImagesInfo& getImagesInfo);
		
	private:
		VkSurfaceKHR surface = nullptr;
		VkSwapchainKHR swapchain = nullptr;
		VkSurfaceFormatKHR surfaceFormat{};
		VkPresentModeKHR presentMode{};

		GTSL::uint8 imageIndex = 0;

		VkSurfaceFormatKHR findFormat(class VulkanRenderDevice* device, VkSurfaceKHR surface);
		VkPresentModeKHR findPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	};
}
