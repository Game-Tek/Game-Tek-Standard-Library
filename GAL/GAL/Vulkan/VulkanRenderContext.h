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

		void CheckSupported();
		
		void Recreate(const RecreateInfo& resizeInfo);
		/**
		 * \brief  Acquires the next image in the swapchain queue to present to.
		 * \param acquireNextImageInfo Information to perform image acquisition.
		 * \return Returns true if the contexts needs to be recreated.
		 */
		[[nodiscard]] GTSL::uint8 AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo);
		void Present(const PresentInfo& presentInfo);
		
		struct GetImagesInfo : RenderInfo
		{
			GTSL::uint32 SwapchainImagesFormat{ 0 };
		};
		GTSL::Array<VulkanImageView, 5> GetImages(const GetImagesInfo& getImagesInfo);
		
	private:
		uint64_t surface{ 0 };
		uint64_t swapchain{ 0 };
	};
}
