#pragma once

#include "GAL/RenderContext.h"

#include "VulkanTexture.h"
#include <GTSL/Pair.h>

#include "GTSL/Array.hpp"

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
#if _WIN64
			WindowsWindowData SystemData;
#endif
		};
		void Initialize(const CreateInfo& createInfo);

		void Destroy(class VulkanRenderDevice* renderDevice);

		GTSL::Array<GTSL::Pair<VulkanColorSpace, VulkanTextureFormat>, 16> GetSupportedFormatsAndColorSpaces(const class VulkanRenderDevice* renderDevice) const;

		GTSL::Array<PresentModes, 4> GetSupportedPresentModes(class VulkanRenderDevice* renderDevice) const;

		struct SurfaceCapabilities
		{
			uint32_t MinImageCount, MaxImageCount;
			GTSL::Extent2D CurrentExtent, MinImageExtent, MaxImageExtent;
//			uint32_t                         maxImageArrayLayers;
			//VkSurfaceTransformFlagsKHR       supportedTransforms;
			//VkSurfaceTransformFlagBitsKHR    currentTransform;
			//VkCompositeAlphaFlagsKHR         supportedCompositeAlpha;
			VkImageUsageFlags SupportedUsageFlags;
		};
		bool IsSupported(class VulkanRenderDevice* renderDevice, SurfaceCapabilities* surfaceCapabilities);

		[[nodiscard]] VulkanHandle GetVkSurface() const { return surface; }
		[[nodiscard]] GTSL::uint64 GetHandle() const { return (GTSL::uint64)surface; }
	
	private:
		VulkanHandle surface = nullptr;
	};

	class VulkanRenderContext final : public RenderContext
	{
	public:
		VulkanRenderContext() = default;

		struct CreateInfo : VulkanCreateInfo
		{
			GTSL::Extent2D SurfaceArea;
			GTSL::uint8 DesiredFramesInFlight = 0;
			PresentModes PresentMode;
			VulkanTextureFormat Format;
			VulkanColorSpace ColorSpace;
			VulkanTextureUses TextureUses;
			const Surface* Surface = nullptr;
			const VulkanQueue* Queue = nullptr;
		};
		VulkanRenderContext(const CreateInfo& createInfo);
		~VulkanRenderContext() = default;

		void Destroy(const class VulkanRenderDevice* renderDevice);


		struct RecreateInfo final : VulkanCreateInfo
		{
			GTSL::Extent2D SurfaceArea;
			GTSL::uint8 DesiredFramesInFlight = 0;
			PresentModes PresentMode;
			VulkanTextureFormat Format;
			VulkanColorSpace ColorSpace;
			VulkanTextureUses TextureUses;
			const VulkanSurface* Surface = nullptr;
			const VulkanQueue* Queue = nullptr;
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
			GTSL::Range<const VulkanSemaphore*> WaitSemaphores;
			GTSL::uint32 ImageIndex = 0;
			const VulkanQueue* Queue = nullptr;
		};
		void Present(const PresentInfo& presentInfo);
		
		struct GetTextureViewsInfo : VulkanRenderInfo
		{
			GTSL::Range<const VulkanTextureView::CreateInfo*> TextureViewCreateInfos;
		};
		GTSL::Array<VulkanTextureView, 8> GetTextureViews(const GetTextureViewsInfo& getTextureViewsInfo) const;

		struct GetTexturesInfo : VulkanRenderInfo
		{
		};
		GTSL::Array<VulkanTexture, 8> GetTextures(const GetTexturesInfo& info) const;
		
	private:
		void* swapchain = nullptr;
	};
}
