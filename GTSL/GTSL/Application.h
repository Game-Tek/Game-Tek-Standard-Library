#pragma once

#include "Core.h"
#include "Delegate.hpp"

namespace GTSL
{
	template<uint32 N>
	class StaticString;
	
	class Window;

	class Application
	{
	public:
		static constexpr uint16 MAX_PATH_LENGTH{ 260 };
		
	private:
		void* handle{ nullptr };
		
	public:
		struct ApplicationCreateInfo
		{
		};
		explicit Application(const ApplicationCreateInfo& applicationCreateInfo);
		~Application();

		void Update();

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
		
		[[nodiscard]] StaticString<MAX_PATH_LENGTH> GetPathToExecutable() const;
	};
}
