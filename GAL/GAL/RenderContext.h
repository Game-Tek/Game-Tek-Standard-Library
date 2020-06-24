#pragma once

#include "RenderCore.h"

#include <GTSL/Extent.h>

#include "GTSL/Ranger.h"

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
			PresentMode PresentMode;
			void* SystemData{ nullptr };
		};
		//explicit RenderContext(const CreateInfo& createInfo);
		
		~RenderContext() = default;

		struct RecreateInfo : RenderInfo
		{
			GTSL::Extent2D NewWindowSize;
			GTSL::uint8 DesiredFramesInFlight = 0;
			PresentMode NewPresentMode;
		};
		void Recreate(const RecreateInfo& resizeInfo);

		struct AcquireNextImageInfo : RenderInfo
		{
			class Semaphore* Semaphore{ nullptr };
			class Fence* Fence{ nullptr };
		};
		void AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo);

		struct PresentInfo : RenderInfo
		{
			Queue* Queue = nullptr;
			GTSL::Ranger<class Semaphore*> WaitSemaphores;
		};
		void Present(const PresentInfo& presentInfo);

		[[nodiscard]] GTSL::uint8 GetMaxFramesInFlight() const { return maxFramesInFlight; }

	protected:
		GTSL::uint8 maxFramesInFlight = 0;

		GTSL::Extent2D extent{ 0, 0 };
	};
}
