#pragma once
#include "Core.h"
#include "DataSizes.h"
#include "Extent.h"
#include "Range.hpp"
#include "StringCommon.h"

#ifdef _WIN64
#undef min
#undef max
#include <Windows.h>
#include <shellapi.h>
#include <VersionHelpers.h>
#elif __linux__
#include <cpuid.h>
#include <sys/sysinfo.h>
#endif

namespace GTSL
{
	struct RamInfo {
		Byte TotalPhysicalMemory;
		Byte FreePhysicalMemory;
		Byte ProcessAvailableMemory;
	};

	struct CPUVectorInfo {
		bool HW_MMX = false, HW_x64 = false, HW_ABM = false, HW_RDRAND = false, HW_BMI1 = false, HW_BMI2 = false, HW_ADX = false, HW_PREFETCHWT1 = false, HW_MPX = false;

		//  SIMD: 128-bit
		bool HW_SSE = false, HW_SSE2 = false, HW_SSE3 = false, HW_SSSE3 = false, HW_SSE41 = false, HW_SSE42 = false, HW_SSE4a = false, HW_AES = false, HW_SHA = false;

		//  SIMD: 256-bit
		bool HW_AVX = false, HW_XOP = false, HW_FMA3 = false, HW_FMA4 = false, HW_AVX2 = false;

		//  SIMD: 512-bit
		bool HW_AVX512_F = false, HW_AVX512_PF = false, HW_AVX512_ER = false, HW_AVX512_CD = false, HW_AVX512_VL = false, HW_AVX512_BW = false, HW_AVX512_DQ = false, HW_AVX512_IFMA = false, HW_AVX512_VBMI = false;
	};

	struct SystemInfo {
		struct CPUInfo {
			CPUVectorInfo VectorInfo;
			uint8 CoreCount = 0;
		} CPU;

		RamInfo RAM;
	};

	class System {
	public:
		static RamInfo GetRAMInfo() {
			RamInfo ramInfo;
#if _WIN64
			MEMORYSTATUSEX memoryStatus{}; memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
			GlobalMemoryStatusEx(&memoryStatus);

			//GTSL_ASSERT(memory_status, "Failed to get Win32 memory status!");

			ramInfo.TotalPhysicalMemory = GTSL::Byte(memoryStatus.ullTotalPhys);
			ramInfo.FreePhysicalMemory = GTSL::Byte(memoryStatus.ullAvailPhys);
			ramInfo.ProcessAvailableMemory = GTSL::Byte(memoryStatus.ullAvailPageFile);
#elif __linux__
			struct sysinfo sys_info;
			sysinfo(&sys_info);

			ramInfo.TotalPhysicalMemory = GTSL::Byte(sys_info.totalram);
			ramInfo.FreePhysicalMemory = GTSL::Byte(sys_info.freeram - sys_info.bufferram);
			ramInfo.ProcessAvailableMemory = GTSL::Byte(sys_info.freeram - sys_info.bufferram);
#endif
			return ramInfo;
		}

		static CPUVectorInfo GetVectorInfo() {
			CPUVectorInfo vectorInfo;
#if _WIN64
			//https://stackoverflow.com/questions/6121792/how-to-check-if-a-cpu-supports-the-sse3-instruction-set

			int info[4];
			__cpuidex(info, 0, 0);
			const int nIds = info[0];

			__cpuidex(info, 0x8000000, 0);
			const uint32 nExIds = info[0];

			//  Detect Features
			if (nIds >= 0x00000001) {
				__cpuidex(info, 0x00000001, 0);
				vectorInfo.HW_MMX = (info[3] & (static_cast<int>(1) << 23)) != 0;
				vectorInfo.HW_SSE = (info[3] & (static_cast<int>(1) << 25)) != 0;
				vectorInfo.HW_SSE2 = (info[3] & (static_cast<int>(1) << 26)) != 0;
				vectorInfo.HW_SSE3 = (info[2] & (static_cast<int>(1) << 0)) != 0;

				vectorInfo.HW_SSSE3 = (info[2] & (static_cast<int>(1) << 9)) != 0;
				vectorInfo.HW_SSE41 = (info[2] & (static_cast<int>(1) << 19)) != 0;
				vectorInfo.HW_SSE42 = (info[2] & (static_cast<int>(1) << 20)) != 0;
				vectorInfo.HW_AES = (info[2] & (static_cast<int>(1) << 25)) != 0;

				vectorInfo.HW_AVX = (info[2] & (static_cast<int>(1) << 28)) != 0;
				vectorInfo.HW_FMA3 = (info[2] & (static_cast<int>(1) << 12)) != 0;

				vectorInfo.HW_RDRAND = (info[2] & (static_cast<int>(1) << 30)) != 0;
			}
			if (nIds >= 0x00000007) {
				__cpuidex(info, 0x00000007, 0);
				vectorInfo.HW_AVX2 = (info[1] & (static_cast<int>(1) << 5)) != 0;

				vectorInfo.HW_BMI1 = (info[1] & (static_cast<int>(1) << 3)) != 0;
				vectorInfo.HW_BMI2 = (info[1] & (static_cast<int>(1) << 8)) != 0;
				vectorInfo.HW_ADX = (info[1] & (static_cast<int>(1) << 19)) != 0;
				vectorInfo.HW_MPX = (info[1] & (static_cast<int>(1) << 14)) != 0;
				vectorInfo.HW_SHA = (info[1] & (static_cast<int>(1) << 29)) != 0;
				vectorInfo.HW_PREFETCHWT1 = (info[2] & (static_cast<int>(1) << 0)) != 0;

				vectorInfo.HW_AVX512_F = (info[1] & (static_cast<int>(1) << 16)) != 0;
				vectorInfo.HW_AVX512_CD = (info[1] & (static_cast<int>(1) << 28)) != 0;
				vectorInfo.HW_AVX512_PF = (info[1] & (static_cast<int>(1) << 26)) != 0;
				vectorInfo.HW_AVX512_ER = (info[1] & (static_cast<int>(1) << 27)) != 0;
				vectorInfo.HW_AVX512_VL = (info[1] & (static_cast<int>(1) << 31)) != 0;
				vectorInfo.HW_AVX512_BW = (info[1] & (static_cast<int>(1) << 30)) != 0;
				vectorInfo.HW_AVX512_DQ = (info[1] & (static_cast<int>(1) << 17)) != 0;
				vectorInfo.HW_AVX512_IFMA = (info[1] & (static_cast<int>(1) << 21)) != 0;
				vectorInfo.HW_AVX512_VBMI = (info[2] & (static_cast<int>(1) << 1)) != 0;
			}
			if (nExIds >= 0x80000001) {
				__cpuidex(info, 0x80000001, 0);
				vectorInfo.HW_x64 = (info[3] & (static_cast<int>(1) << 29)) != 0;
				vectorInfo.HW_ABM = (info[2] & (static_cast<int>(1) << 5)) != 0;
				vectorInfo.HW_SSE4a = (info[2] & (static_cast<int>(1) << 6)) != 0;
				vectorInfo.HW_FMA4 = (info[2] & (static_cast<int>(1) << 16)) != 0;
				vectorInfo.HW_XOP = (info[2] & (static_cast<int>(1) << 11)) != 0;
			}
#elif __linux__
			uint32 info[4] = {0};
			__cpuid(0, info[0], info[1], info[2], info[3]);
			const uint32 nIds = info[0];

			__cpuid(0x80000000, info[0], info[1], info[2], info[3]);
			const uint32 nExIds = info[0];

			//  Detect Features
			if (nIds >= 0x00000001) {
				__cpuid(0x00000001, info[0], info[1], info[2], info[3]);
				vectorInfo.HW_MMX = (info[3] & (static_cast<int>(1) << 23)) != 0;
				vectorInfo.HW_SSE = (info[3] & (static_cast<int>(1) << 25)) != 0;
				vectorInfo.HW_SSE2 = (info[3] & (static_cast<int>(1) << 26)) != 0;
				vectorInfo.HW_SSE3 = (info[2] & (static_cast<int>(1) << 0)) != 0;

				vectorInfo.HW_SSSE3 = (info[2] & (static_cast<int>(1) << 9)) != 0;
				vectorInfo.HW_SSE41 = (info[2] & (static_cast<int>(1) << 19)) != 0;
				vectorInfo.HW_SSE42 = (info[2] & (static_cast<int>(1) << 20)) != 0;
				vectorInfo.HW_AES = (info[2] & (static_cast<int>(1) << 25)) != 0;

				vectorInfo.HW_AVX = (info[2] & (static_cast<int>(1) << 28)) != 0;
				vectorInfo.HW_FMA3 = (info[2] & (static_cast<int>(1) << 12)) != 0;

				vectorInfo.HW_RDRAND = (info[2] & (static_cast<int>(1) << 30)) != 0;
			}
			if (nIds >= 0x00000007) {
				__cpuid_count(0x00000007, 0, info[0], info[1], info[2], info[3]);

				vectorInfo.HW_AVX2 = (info[1] & (static_cast<int>(1) << 5)) != 0;

				vectorInfo.HW_BMI1 = (info[1] & (static_cast<int>(1) << 3)) != 0;
				vectorInfo.HW_BMI2 = (info[1] & (static_cast<int>(1) << 8)) != 0;
				vectorInfo.HW_ADX = (info[1] & (static_cast<int>(1) << 19)) != 0;
				vectorInfo.HW_MPX = (info[1] & (static_cast<int>(1) << 14)) != 0;
				vectorInfo.HW_SHA = (info[1] & (static_cast<int>(1) << 29)) != 0;
				vectorInfo.HW_PREFETCHWT1 = (info[2] & (static_cast<int>(1) << 0)) != 0;

				vectorInfo.HW_AVX512_F = (info[1] & (static_cast<int>(1) << 16)) != 0;
				vectorInfo.HW_AVX512_CD = (info[1] & (static_cast<int>(1) << 28)) != 0;
				vectorInfo.HW_AVX512_PF = (info[1] & (static_cast<int>(1) << 26)) != 0;
				vectorInfo.HW_AVX512_ER = (info[1] & (static_cast<int>(1) << 27)) != 0;
				vectorInfo.HW_AVX512_VL = (info[1] & (static_cast<int>(1) << 31)) != 0;
				vectorInfo.HW_AVX512_BW = (info[1] & (static_cast<int>(1) << 30)) != 0;
				vectorInfo.HW_AVX512_DQ = (info[1] & (static_cast<int>(1) << 17)) != 0;
				vectorInfo.HW_AVX512_IFMA = (info[1] & (static_cast<int>(1) << 21)) != 0;
				vectorInfo.HW_AVX512_VBMI = (info[2] & (static_cast<int>(1) << 1)) != 0;
			}
			if (nExIds >= 0x80000001) {
				__cpuid_count(0x80000001, 0, info[0], info[1], info[2], info[3]);

				vectorInfo.HW_x64 = (info[3] & (static_cast<int>(1) << 29)) != 0;
				vectorInfo.HW_ABM = (info[2] & (static_cast<int>(1) << 5)) != 0;
				vectorInfo.HW_SSE4a = (info[2] & (static_cast<int>(1) << 6)) != 0;
				vectorInfo.HW_FMA4 = (info[2] & (static_cast<int>(1) << 16)) != 0;
				vectorInfo.HW_XOP = (info[2] & (static_cast<int>(1) << 11)) != 0;
			}
#endif
			return vectorInfo;
		}

		static SystemInfo GetSystemInfo() {
			SystemInfo systemInfo;
#if _WIN64
			SYSTEM_INFO system_info;
			::GetSystemInfo(&system_info);
			systemInfo.CPU.CoreCount = static_cast<uint8>(system_info.dwNumberOfProcessors);
#elif __linux__
			systemInfo.CPU.CoreCount = static_cast<uint8>(sysconf(_SC_NPROCESSORS_ONLN));
#endif

			systemInfo.CPU.VectorInfo = GetVectorInfo();
			systemInfo.RAM = GetRAMInfo();

			return systemInfo;
		}

		static void QueryParameter(const Range<const char8_t*> name) {
#ifdef _WIN32
			HKEY result;
			auto openResult = RegOpenKeyA(HKEY_LOCAL_MACHINE, "SOFTWARE\\", &result);
			if(openResult != ERROR_SUCCESS) {}
			DWORD valueCount = 0, maxNameLength = 0, maxValueLength = 0;
			openResult = RegQueryInfoKeyA(result, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &valueCount, &maxNameLength, &maxValueLength, nullptr, nullptr);
			if(openResult != ERROR_SUCCESS) {}
			DWORD type = 0, index = 0, charCountValueName = 128, charBytesData = 1024;
			char8_t valueName[128]; byte dataBuffer[1024];
			RegEnumValueA(result, index, reinterpret_cast<char*>(valueName), &charCountValueName, nullptr, &type, dataBuffer, &charBytesData);
			RegCloseKey(result);
#endif
		}

		static bool Run(StringView path) {
#ifdef _WIN32
			return ShellExecuteA(nullptr, "open", reinterpret_cast<const char*>(path.GetData()), nullptr, nullptr, SW_SHOWNORMAL);
#elif __linux__
            return false;
#endif
		}

		static bool CheckOSVersion() {
#ifdef _WIN32
			return IsWindowsVersionOrGreater(10, 5, 5);
#endif

            return true;
		}
	};

	class Console {
	public:
		static void SetConsoleInputModeAsUTF8() {
#ifdef _WIN32
			SetConsoleOutputCP(CP_UTF8);
#endif
		}

		static void Print(const Range<const char8_t*> text) {
#ifdef _WIN32
			WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), text.GetData(), text.GetBytes(), nullptr, nullptr);
#elif __linux__
			write(1, reinterpret_cast<const void*>(text.GetData()), text.GetBytes());
#endif
		}

		//static void Read(Range<char8_t*> betaTable) {
		//	DWORD characters_read{ 0 };
		//	ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), betaTable.begin(), betaTable.ElementCount(), &characters_read, nullptr);
		//}

		static constexpr char8_t BLACK[] = u8"\u001b[30m";
		static constexpr char8_t RED[] = u8"\033[31m";
		static constexpr char8_t GREEN[] = u8"\033[32m";
		static constexpr char8_t YELLOW[] = u8"\033[33m";
		static constexpr char8_t BLUE[] = u8"\033[34m";
		static constexpr char8_t MAGENTA[] = u8"\033[35m";
		static constexpr char8_t CYAN[] = u8"\033[36m";
		static constexpr char8_t WHITE[] = u8"\u001b[37m";
		static constexpr char8_t ORANGE[] = u8"\u001b[38;5;208m";
		static constexpr char8_t GRAY[] = u8"\u001b[38;5;244m";
		static constexpr char8_t RESET[] = u8"\033[0m";

		static constexpr char8_t BOLD[] = u8"\033[1m";
		static constexpr char8_t UNDERLINE[] = u8"\033[4m";
		static constexpr char8_t BLINK[] = u8"\033[5m";
		static constexpr char8_t INVERT[] = u8"\033[7m";
		static constexpr char8_t CROSS[] = u8"\033[9m";

		static constexpr char8_t REMOVE_BOLD[] = u8"\033[21m";
		static constexpr char8_t REMOVE_UNDERLINE[] = u8"\033[24m";
		static constexpr char8_t REMOVE_BLINK[] = u8"\033[25m";
		static constexpr char8_t REMOVE_INVERT[] = u8"\033[27m";
		static constexpr char8_t REMOVE_CROSS[] = u8"\033[29m";

		enum class TextColor : uint8 {
			BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, ORANGE, GRAY, RESET
		};
		static void SetTextColor(TextColor textColor) {
#ifdef _WIN32
			WORD color{ 0 };

			switch (textColor) {
				case TextColor::BLACK:	color = 0; break;
				case TextColor::RED:	color = FOREGROUND_INTENSITY | FOREGROUND_RED; break;
				case TextColor::GREEN:	color = FOREGROUND_INTENSITY | FOREGROUND_GREEN; break;
				case TextColor::YELLOW:	color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN; break;
				case TextColor::BLUE:	color = FOREGROUND_INTENSITY | FOREGROUND_BLUE; break;
				case TextColor::MAGENTA:color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE; break;
				case TextColor::CYAN:	color = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
				case TextColor::WHITE:	color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
				case TextColor::ORANGE:	color = 					   FOREGROUND_RED | FOREGROUND_GREEN; break;
				case TextColor::GRAY:	color = 					   FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
				case TextColor::RESET:	color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
				default: break;
			}

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
#elif __linux__
			switch (textColor) {
				case TextColor::BLACK:    write(1, BLACK, 6ull); break;
				case TextColor::RED:      write(1, RED, 6ull); break;
				case TextColor::GREEN:    write(1, GREEN, 6ull); break;
				case TextColor::YELLOW:   write(1, YELLOW, 6ull); break;
				case TextColor::BLUE:     write(1, BLUE, 6ull); break;
				case TextColor::MAGENTA:  write(1, MAGENTA, 6ull); break;
				case TextColor::CYAN:     write(1, CYAN, 6ull); break;
				case TextColor::WHITE:    write(1, WHITE, 6ull); break;
				case TextColor::ORANGE:   write(1, ORANGE, 12ull); break;
				case TextColor::GRAY:     write(1, GRAY, 12ull); break;
				case TextColor::RESET:    write(1, RESET, 5ull); break;
				default: break;
			}
		}
#endif

		enum class TextEffect : uint8 {
			BOLD, UNDERLINE, BLINK, INVERT, CROSS, RESET
		};
		static void SetTextEffect(TextEffect textEffect) {
#if __linux__
			switch (textEffect) {
				case TextEffect::BOLD:      write(1, BOLD, 5ull); break;
				case TextEffect::UNDERLINE: write(1, UNDERLINE, 5ull); break;
				case TextEffect::BLINK:     write(1, BLINK, 5ull); break;
				case TextEffect::INVERT:    write(1, INVERT, 5ull); break;
				case TextEffect::CROSS:     write(1, CROSS, 5ull); break;
				case TextEffect::RESET:     write(1, RESET, 5ull); break;
				default: break;
			}
#endif
		}
	};
}
