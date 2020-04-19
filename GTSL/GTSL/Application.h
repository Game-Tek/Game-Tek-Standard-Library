#pragma once

namespace GTSL
{
	class Window;

	class Application
	{

	public:
		struct ApplicationCreateInfo
		{
		};
		explicit Application(const ApplicationCreateInfo& applicationCreateInfo);
		~Application();

		void Update();

		void UpdateWindow(Window* window);

		void Close();

		struct Win32NativeHandles
		{
			void* HINSTANCE{ nullptr };
		};
		void GetNativeHandles(void* nativeHandles);
	};
}