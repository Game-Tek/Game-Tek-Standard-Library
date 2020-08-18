#pragma once

#include "GAL/RenderContext.h"

#include "VulkanBindings.h"
#include "VulkanImage.h"
#include <GTSL/Pair.h>

namespace GAL
{
	class VulkanQueue;
	class VulkanSemaphore;

	class VulkanSurface final : public Surface
	{
	public:
		VulkanSurface() = default;
		
		struct CreateInfo final : VulkanCreateInfo
		{
			void* SystemData{ nullptr };
		};
		VulkanSurface(const CreateInfo& createInfo);

		void Destroy(class VulkanRenderDevice* renderDevice);

		//colorspace / format
		GTSL::uint32 GetSupportedRenderContextFormat(class VulkanRenderDevice* renderDevice, GTSL::Ranger<GTSL::Pair<GTSL::uint32, GTSL::uint32>> formats);

		GTSL::uint32 GetSupportedPresentMode(class VulkanRenderDevice* renderDevice, GTSL::Ranger<GTSL::uint32> presentModes);

		bool IsSupported(class VulkanRenderDevice* renderDevice);

		[[nodiscard]] VulkanHandle GetVkSurface() const { return surface; }
	private:
		VulkanHandle surface{ 0 };
	};

	class VulkanRenderContext final : public RenderContext
	{
	public:
		VulkanRenderContext() = default;

		struct CreateInfo : VulkanCreateInfo
		{
			GTSL::Extent2D SurfaceArea;
			GTSL::uint8 DesiredFramesInFlight = 0;
			GTSL::uint32 PresentMode{ 0 };
			GTSL::uint32 Format{ 0 };
			GTSL::uint32 ColorSpace{ 0 };
			GTSL::uint32 ImageUses{ 0 };
			const Surface* Surface{ nullptr };
		};
		VulkanRenderContext(const CreateInfo& createInfo);
		~VulkanRenderContext() = default;

		void Destroy(const class VulkanRenderDevice* renderDevice);


		struct RecreateInfo : VulkanCreateInfo
		{
			GTSL::Extent2D SurfaceArea;
			GTSL::uint8 DesiredFramesInFlight = 0;
			GTSL::uint32 PresentMode{ 0 };
			GTSL::uint32 Format{ 0 };
			GTSL::uint32 ColorSpace{ 0 };
			GTSL::uint32 ImageUses{ 0 };
			const VulkanSurface* Surface{ nullptr };
		};
		void Recreate(const RecreateInfo& resizeInfo);

		struct AcquireNextImageInfo : VulkanRenderInfo
		{
			VulkanSemaphore* SignalSemaphore{ nullptr };
			const class VulkanFence* Fence{ nullptr };
		};
		/**
		 * \brief  Acquires the next image in the swapchain queue to present to.
		 * \param acquireNextImageInfo Information to perform image acquisition.
		 * \return Returns true if the contexts needs to be recreated.
		 */
		[[nodiscard]] GTSL::uint8 AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo);

		struct PresentInfo final : VulkanRenderInfo
		{
			GTSL::Ranger<const VulkanSemaphore> WaitSemaphores;
			GTSL::uint32 ImageIndex = 0;
			const VulkanQueue* Queue = nullptr;
		};
		void Present(const PresentInfo& presentInfo);
		
		struct GetImagesInfo : VulkanRenderInfo
		{
			GTSL::uint32 SwapchainImagesFormat{ 0 };
			GTSL::Ranger<const GTSL::UTF8> ImageViewName;
			GTSL::Ranger<const VulkanImageView::CreateInfo> ImageViewCreateInfos;
		};
		GTSL::Array<VulkanImageView, 5> GetImages(const GetImagesInfo& getImagesInfo);
		
	private:
		void* swapchain{ 0 };
	};
}
