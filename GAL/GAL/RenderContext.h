#pragma once

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
	
	class Surface
	{
	public:
		Surface() = default;
	};

	class RenderContext
	{
	public:
		RenderContext() = default;

		//explicit RenderContext(const CreateInfo& createInfo);
		
		~RenderContext() = default;
	};
}
