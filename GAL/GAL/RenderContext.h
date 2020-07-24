#pragma once

#include "RenderCore.h"

#include <GTSL/Extent.h>
#include <GTSL/Ranger.h>

#include "Vulkan/VulkanSynchronization.h"

namespace GAL
{
	class Window;
	class RenderDevice;
	class Queue;
	class RenderTarget;

#if (_WIN32)
	struct WindowsWindowData
	{
		void* WindowHandle{nullptr};
		void* InstanceHandle{nullptr};
	};
#endif
	
	class Surface : public GALObject
	{
	public:
		Surface() = default;
	};

	class RenderContext : public GALObject
	{
	public:
		RenderContext() = default;
		
		struct CreateInfo : RenderInfo
		{
			GTSL::Extent2D SurfaceArea;
			GTSL::uint8 DesiredFramesInFlight = 0;
			GTSL::uint32 PresentMode{ 0 };
			GTSL::uint32 Format{ 0 };
			GTSL::uint32 ColorSpace{ 0 };
			GTSL::uint32 ImageUses{ 0 };
			const Surface* Surface{ nullptr };
		};
		//explicit RenderContext(const CreateInfo& createInfo);
		
		~RenderContext() = default;

		struct RecreateInfo : RenderInfo
		{
			GTSL::Extent2D SurfaceArea;
			GTSL::uint8 DesiredFramesInFlight = 0;
			GTSL::uint32 PresentMode{ 0 };
			GTSL::uint32 Format{ 0 };
			GTSL::uint32 ColorSpace{ 0 };
			GTSL::uint32 ImageUses{ 0 };
			const Surface* Surface{ nullptr };
		};
		void Recreate(const RecreateInfo& resizeInfo);

		struct AcquireNextImageInfo : RenderInfo
		{
			VulkanSemaphore* Semaphore{nullptr};
			const class Fence* Fence{ nullptr };
		};
		void AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo);

		struct PresentInfo : RenderInfo
		{
			const Queue* Queue = nullptr;
			GTSL::Ranger<const class Semaphore> WaitSemaphores;
			GTSL::uint8 ImageIndex{ 0 };
		};
		void Present(const PresentInfo& presentInfo);
	};
}
