#pragma once

#include "RenderCore.h"

#include <GTSL/Extent.h>

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
	
	class RenderContext : public GALObject
	{
	public:
		RenderContext() = default;
		struct CreateInfo : RenderInfo
		{
			GTSL::Extent2D SurfaceArea;
			GTSL::uint8 DesiredFramesInFlight = 0;
			void* SystemData{ nullptr };
		};
		explicit RenderContext(const CreateInfo& createInfo);
		
		~RenderContext() = default;

		struct ResizeInfo : RenderInfo
		{
			GTSL::Extent2D NewWindowSize;
		};
		void OnResize(const ResizeInfo& resizeInfo);

		struct AcquireNextImageInfo : RenderInfo
		{
		};
		void AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo);

		struct FlushInfo : RenderInfo
		{
			Queue* Queue = nullptr;
			class CommandBuffer* CommandBuffer = nullptr;
		};
		void Flush(const FlushInfo& flushInfo);

		struct PresentInfo : RenderInfo
		{
			Queue* Queue = nullptr;
		};
		void Present(const PresentInfo& presentInfo);

		[[nodiscard]] GTSL::uint8 GetCurrentImage() const { return currentImage; }
		[[nodiscard]] GTSL::uint8 GetMaxFramesInFlight() const { return maxFramesInFlight; }

	protected:
		GTSL::uint8 currentImage = 0;
		GTSL::uint8 maxFramesInFlight = 0;

		GTSL::Extent2D extent{ 0, 0 };
	};
}
