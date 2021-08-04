#pragma once

#include "Core.h"
#include "String.hpp"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#define NO_COMM
#include <Windows.h>
#endif

namespace GTSL
{	
	class Window;

	class Application
	{
	public:
		static constexpr uint16 MAX_PATH_LENGTH{ 260 };
		
	private:
		HINSTANCE handle{ nullptr };
		
	public:
		struct ApplicationCreateInfo
		{
		};
		Application(const ApplicationCreateInfo& applicationCreateInfo) : handle(GetModuleHandleA(nullptr))
		{
		}
		
		~Application() = default;
		
		HINSTANCE GetHINSTANCE() const { return handle; }

		enum class Priority : uint8
		{
			LOW, LOW_MID, MID, MID_HIGH, HIGH
		};
		void SetProcessPriority(Priority priority) noexcept
		{
			int32 priority_class{ NORMAL_PRIORITY_CLASS };
			switch (priority)
			{
			case Priority::LOW: priority_class = IDLE_PRIORITY_CLASS; break;
			case Priority::LOW_MID: priority_class = BELOW_NORMAL_PRIORITY_CLASS; break;
			case Priority::MID: priority_class = NORMAL_PRIORITY_CLASS; break;
			case Priority::MID_HIGH: priority_class = ABOVE_NORMAL_PRIORITY_CLASS; break;
			case Priority::HIGH: priority_class = HIGH_PRIORITY_CLASS; break;
			default: break;
			}

			SetPriorityClass(GetCurrentProcess(), priority_class);
		}
		
		void Close()
		{
			PostQuitMessage(0);
		}

		struct NativeHandles
		{
#if (_WIN64)
			HINSTANCE HINSTANCE{ nullptr };
#endif
		};
		
		void GetNativeHandles(NativeHandles* nativeHandles) {
#if (_WIN64)
			nativeHandles->HINSTANCE = handle;
#endif
		}

		static uint8 ThreadCount() noexcept
		{
			SYSTEM_INFO system_info; GetSystemInfo(&system_info); return static_cast<uint8>(system_info.dwNumberOfProcessors);
		}
		
		[[nodiscard]] StaticString<MAX_PATH_LENGTH> GetPathToExecutable() const
		{
			char s[MAX_PATH_LENGTH];
			GetModuleFileNameA(handle, s, MAX_PATH_LENGTH);
			
			StaticString<MAX_PATH_LENGTH> ret(reinterpret_cast<char8_t*>(s));
			ret.ReplaceAll('\\', '/');
			return ret;
		}
	};
}
