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

		//explicit RenderContext(const CreateInfo& createInfo);
		
		~RenderContext() = default;

		struct PresentInfo : RenderInfo
		{
			const Queue* Queue = nullptr;
			GTSL::Ranger<const class Semaphore> WaitSemaphores;
			GTSL::uint8 ImageIndex{ 0 };
		};
		void Present(const PresentInfo& presentInfo);
	};
}
