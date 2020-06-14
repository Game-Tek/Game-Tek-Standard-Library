#pragma once

#include "RenderCore.h"

#include <GTSL/Extent.h>
#include <GTSL/Vector.hpp>
#include <GTSL/Array.hpp>

#if (_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace GAL
{
	class Window;
	class RenderDevice;
	class Queue;
	class RenderTarget;

	struct ResizeInfo
	{
		RenderDevice* RenderDevice = nullptr;
		GTSL::Extent2D NewWindowSize;
	};

#if (_WIN32)
	struct WindowsWindowData
	{
		HWND WindowHandle;
		HINSTANCE InstanceHandle;
	};
#endif
	
	class RenderContext : public GALObject
	{
	protected:
		GTSL::uint8 currentImage = 0;
		GTSL::uint8 maxFramesInFlight = 0;

		GTSL::Extent2D extent{ 0, 0 };

	public:
		struct CreateInfo : RenderInfo
		{
			GTSL::Extent2D SurfaceArea;
			void* SystemData{ nullptr };
			GTSL::uint8 DesiredFramesInFlight = 0;
		};
		explicit RenderContext(const CreateInfo& createInfo);
		virtual ~RenderContext() = default;

		void OnResize(const ResizeInfo& _RI);

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
	};
}
