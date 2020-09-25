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
			WindowsWindowData* SystemData;
		};
		VulkanSurface(const CreateInfo& createInfo);

		void Destroy(class VulkanRenderDevice* renderDevice);

		GTSL::uint32 GetSupportedRenderContextFormat(const class VulkanRenderDevice* renderDevice, GTSL::Range<const GTSL::Pair<VulkanColorSpace, VulkanTextureFormat>*> formats) const;

		GTSL::uint32 GetSupportedPresentMode(class VulkanRenderDevice* renderDevice, GTSL::Range<const VulkanPresentMode*> presentModes);

		bool IsSupported(class VulkanRenderDevice* renderDevice);

		[[nodiscard]] VulkanHandle GetVkSurface() const { return surface; }
	private:
		VulkanHandle surface;
	};

	class VulkanRenderContext final : public RenderContext
	{
	public:
		VulkanRenderContext() = default;

		struct CreateInfo : VulkanCreateInfo
		{
			GTSL::Extent2D SurfaceArea;
			GTSL::uint8 DesiredFramesInFlight = 0;
			VulkanPresentMode PresentMode;
			VulkanTextureFormat Format;
			VulkanColorSpace ColorSpace;
			VulkanTextureUses::value_type TextureUses;
			const Surface* Surface{ nullptr };
		};
		VulkanRenderContext(const CreateInfo& createInfo);
		~VulkanRenderContext() = default;

		void Destroy(const class VulkanRenderDevice* renderDevice);


		struct RecreateInfo final : VulkanCreateInfo
		{
			GTSL::Extent2D SurfaceArea;
			GTSL::uint8 DesiredFramesInFlight = 0;
			VulkanPresentMode PresentMode;
			VulkanTextureFormat Format;
			VulkanColorSpace ColorSpace{ 0 };
			VulkanTextureUses::value_type TextureUses;
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
			GTSL::Range<const VulkanSemaphore*> WaitSemaphores;
			GTSL::uint32 ImageIndex = 0;
			const VulkanQueue* Queue = nullptr;
		};
		void Present(const PresentInfo& presentInfo);
		
		struct GetTextureViewsInfo : VulkanRenderInfo
		{
			GTSL::Range<const VulkanTextureView::CreateInfo*> TextureViewCreateInfos;
		};
		GTSL::Array<VulkanTextureView, 5> GetTextureViews(const GetTextureViewsInfo& getTextureViewsInfo) const;

		struct GetTexturesInfo : VulkanRenderInfo
		{
		};
		GTSL::Array<VulkanTexture, 5> GetTextures(const GetTexturesInfo& info) const;
		
	private:
		void* swapchain = nullptr;
	};
}
