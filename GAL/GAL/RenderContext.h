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
	
	struct RenderContextCreateInfo
	{
		GTSL::Extent2D SurfaceArea;
		void* SystemData{ nullptr };
		GTSL::uint8 DesiredFramesInFlight = 0;
	};
	
	class RenderContext : public GALObject
	{
	protected:
		GTSL::uint8 currentImage = 0;
		GTSL::uint8 maxFramesInFlight = 0;

		GTSL::Extent2D extent{ 0, 0 };

		GTSL::Array<GAL::RenderTarget*, 5, GTSL::uint8> renderTargets;

	public:
		virtual ~RenderContext() = default;

		virtual void OnResize(const ResizeInfo& _RI) = 0;

		struct AcquireNextImageInfo : RenderInfo
		{
		};
		virtual void AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo) = 0;

		struct FlushInfo : RenderInfo
		{
			Queue* Queue = nullptr;
			class CommandBuffer* CommandBuffer = nullptr;
		};
		virtual void Flush(const FlushInfo& flushInfo) = 0;

		struct PresentInfo : RenderInfo
		{
			Queue* Queue = nullptr;
		};
		virtual void Present(const PresentInfo& presentInfo) = 0;

		[[nodiscard]] GTSL::Array<RenderTarget*, 5, GTSL::uint8> GetSwapchainRenderTargets() const { return renderTargets; }

		[[nodiscard]] GTSL::uint8 GetCurrentImage() const { return currentImage; }
		[[nodiscard]] GTSL::uint8 GetMaxFramesInFlight() const { return maxFramesInFlight; }
	};
}
