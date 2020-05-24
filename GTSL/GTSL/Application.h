#pragma once

#include "Ranger.h"
#include "StaticString.hpp"

namespace GTSL
{
	class Window;

	class Application
	{
	public:
		static constexpr uint16 MaxPathLength{ 1024 };
		
	private:
		void* handle{ nullptr };
		
	public:
		struct ApplicationCreateInfo
		{
		};
		explicit Application(const ApplicationCreateInfo& applicationCreateInfo);
		~Application();

		void Update();

		void UpdateWindow(Window* window);

		enum class Priority : uint8
		{
			LOW, LOW_MID, MID, MID_HIGH, HIGH
		};
		void SetProcessPriority(Priority priority) noexcept;
		
		void Close();

		struct Win32NativeHandles
		{
			void* HINSTANCE{ nullptr };
		};
		void GetNativeHandles(void* nativeHandles);

		static uint8 ThreadCount() noexcept;
		
		[[nodiscard]] StaticString<MaxPathLength> GetPathToExecutable() const;
	};
}
