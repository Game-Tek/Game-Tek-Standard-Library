#pragma once

#include "Ranger.h"
#include "StaticString.hpp"

namespace GTSL
{
	class Window;

	class Application
	{
	public:
		static constexpr uint16 MaxPathLength{ 2048 };
		
	private:
		StaticString<MaxPathLength> path;
		void* handle{ nullptr };
		
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

		[[nodiscard]] Ranger<UTF8> GetPathToExecutable() const;
	};
}
