#pragma once

#include "GAL/RenderContext.h"

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
		[[nodiscard]] GTSL::uint8 GetMaxFramesInFlight() const { return maxFramesInFlight; }
		
		struct GetImagesInfo : RenderInfo
		{
			GTSL::uint32 SwapchainImagesFormat{ 0 };
		};
		GTSL::Array<VulkanImage, 5> GetImages(const GetImagesInfo& getImagesInfo);
		
	private:
		uint64_t surface{ 0 };
		uint64_t swapchain{ 0 };

		GTSL::uint8 imageIndex = 0;
		GTSL::uint8 maxFramesInFlight = 0;
	};
}
