#pragma once

#include "Core.h"
#include "String.hpp"

#if (_WIN64)
#include <Windows.h>
#elif __linux__
#include <unistd.h>
#endif

namespace GTSL
{	
	class Window;

	class Application
	{
	public:
		static constexpr uint16 MAX_PATH_LENGTH{ 260 };
		
	private:
#ifdef _WIN32
		HINSTANCE handle{ nullptr };
#endif
		
	public:
		struct ApplicationCreateInfo
		{
		};

#ifdef _WIN32
		Application(const ApplicationCreateInfo& applicationCreateInfo) : handle(GetModuleHandleA(nullptr))
		{
		}
#endif
		
		~Application() = default;
		
#ifdef _WIN32
		HINSTANCE GetHINSTANCE() const { return handle; }
#endif

		enum class Priority : uint8 {
			LOW, LOW_MID, MID, MID_HIGH, HIGH
		};
		void SetProcessPriority(Priority priority) noexcept {
#ifdef _WIN32
			int32 priority_class{ NORMAL_PRIORITY_CLASS };
			switch (priority) {
			case Priority::LOW: priority_class = IDLE_PRIORITY_CLASS; break;
			case Priority::LOW_MID: priority_class = BELOW_NORMAL_PRIORITY_CLASS; break;
			case Priority::MID: priority_class = NORMAL_PRIORITY_CLASS; break;
			case Priority::MID_HIGH: priority_class = ABOVE_NORMAL_PRIORITY_CLASS; break;
			case Priority::HIGH: priority_class = HIGH_PRIORITY_CLASS; break;
			}

			SetPriorityClass(GetCurrentProcess(), priority_class);
#elif __linux__
			int32 niceNess = 0;
			switch (priority) {
			case Priority::LOW: niceNess = 19; break;
			case Priority::LOW_MID: niceNess = 11; break;
			case Priority::MID: niceNess = 0; break;
			case Priority::MID_HIGH: niceNess = -10; break;
			case Priority::HIGH: niceNess = -20; break;
			}
			nice(niceNess);
#endif
		}
		
		void Close() {
#ifdef _WIN32
			PostQuitMessage(0);
#endif
		}

		struct NativeHandles {
#if (_WIN64)
			HINSTANCE HINSTANCE{ nullptr };
#endif
		};
		
		void GetNativeHandles(NativeHandles* nativeHandles) {
#if (_WIN64)
			nativeHandles->HINSTANCE = handle;
#endif
		}

		static uint8 ThreadCount() noexcept {
#ifdef _WIN32
			SYSTEM_INFO system_info; GetSystemInfo(&system_info); return static_cast<uint8>(system_info.dwNumberOfProcessors);
#elif __linux__
			return (uint8)sysconf(_SC_NPROCESSORS_ONLN);
#endif
		}
		
		[[nodiscard]] static StaticString<MAX_PATH_LENGTH> GetPathToExecutable() {
#ifdef _WIN32
			char s[MAX_PATH_LENGTH];
			GetModuleFileNameA(handle, s, MAX_PATH_LENGTH);
			
			StaticString<MAX_PATH_LENGTH> ret(reinterpret_cast<const char8_t*>(s));
			ReplaceAll(ret, u8'\\', u8'/');
			return ret;
#elif __linux__
			char self[MAX_PATH_LENGTH] = { 0 };
			int nchar = readlink("/proc/self/exe", self, MAX_PATH_LENGTH);

			if (nchar < 0) {
				return {};
			}

			return StaticString<MAX_PATH_LENGTH>(GTSL::StringView(GTSL::Byte(nchar), (const char8_t*)self));
#endif
		}
	};
}
