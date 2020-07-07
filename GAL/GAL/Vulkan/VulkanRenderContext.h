#pragma once

#include "GAL/RenderContext.h"

#include "VulkanBindings.h"
#include "VulkanImage.h"
#include <GTSL\Pair.h>

namespace GAL
{
	class VulkanSurface final : public Surface
	{
	public:
		VulkanSurface() = default;
		struct CreateInfo
		{
			class VulkanRenderDevice* RenderDevice{ nullptr };
			void* SystemData{ nullptr };
		};
		VulkanSurface(const CreateInfo& createInfo);

		void Destroy(class VulkanRenderDevice* renderDevice);

		//colorspace / format
		GTSL::uint32 GetSupportedRenderContextFormat(class VulkanRenderDevice* renderDevice, GTSL::Ranger<GTSL::Pair<GTSL::uint32, GTSL::uint32>> formats);

		GTSL::uint32 GetSupportedPresentMode(class VulkanRenderDevice* renderDevice, GTSL::Ranger<GTSL::uint32> presentModes);

		bool IsSupported(class VulkanRenderDevice* renderDevice);

		void* GetVkSurface() const { return surface; }
	private:
		void* surface{ 0 };
	};

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
		[[nodiscard]] GTSL::uint8 AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo);
		void Present(const PresentInfo& presentInfo);
		
		struct GetImagesInfo : RenderInfo
		{
			GTSL::uint32 SwapchainImagesFormat{ 0 };
		};
		GTSL::Array<VulkanImageView, 5> GetImages(const GetImagesInfo& getImagesInfo);
		
	private:
		void* swapchain{ 0 };
	};
}
