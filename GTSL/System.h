#pragma once
#include "Core.h"
#include "DataSizes.h"
#include "Extent.h"
#include "Range.hpp"
#include "StringCommon.h"

#ifdef _WIN64
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#undef min
#undef max
#include <Windows.h>
#endif

namespace GTSL
{
	struct RamInfo
	{
		GTSL::Byte TotalPhysicalMemory;
		GTSL::Byte FreePhysicalMemory;
		GTSL::Byte ProcessAvailableMemory;
	};

	struct VectorInfo
	{
		bool HW_MMX;
		bool HW_x64;
		bool HW_ABM;
		bool HW_RDRAND;
		bool HW_BMI1;
		bool HW_BMI2;
		bool HW_ADX;
		bool HW_PREFETCHWT1;
		bool HW_MPX;

		//  SIMD: 128-bit
		bool HW_SSE;
		bool HW_SSE2;
		bool HW_SSE3;
		bool HW_SSSE3;
		bool HW_SSE41;
		bool HW_SSE42;
		bool HW_SSE4a;
		bool HW_AES;
		bool HW_SHA;

		//  SIMD: 256-bit
		bool HW_AVX;
		bool HW_XOP;
		bool HW_FMA3;
		bool HW_FMA4;
		bool HW_AVX2;

		//  SIMD: 512-bit
		bool HW_AVX512_F;
		bool HW_AVX512_PF;
		bool HW_AVX512_ER;
		bool HW_AVX512_CD;
		bool HW_AVX512_VL;
		bool HW_AVX512_BW;
		bool HW_AVX512_DQ;
		bool HW_AVX512_IFMA;
		bool HW_AVX512_VBMI;
	};

	struct SystemInfo
	{
		struct CPUInfo
		{
			VectorInfo VectorInfo;
			uint8 CoreCount = 0;
		} CPU;

		RamInfo RAM;
	};

	class System
	{
	public:
		static void GetRAMInfo(RamInfo& ramInfo) {
#if (_WIN32)
			MEMORYSTATUSEX memoryStatus{}; memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
			GlobalMemoryStatusEx(&memoryStatus);

			//GTSL_ASSERT(memory_status, "Failed to get Win32 memory status!");

			ramInfo.TotalPhysicalMemory = GTSL::Byte(memoryStatus.ullTotalPhys);
			ramInfo.FreePhysicalMemory = GTSL::Byte(memoryStatus.ullAvailPhys);
			ramInfo.ProcessAvailableMemory = GTSL::Byte(memoryStatus.ullAvailPageFile);
#endif
		}

		static void GetVectorInfo(VectorInfo& vectorInfo) {
#if (_WIN32)
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
#endif
		}

		static void GetSystemInfo(SystemInfo& systemInfo) {
#if (_WIN32)
			SYSTEM_INFO system_info;
			::GetSystemInfo(&system_info);

			systemInfo.CPU.CoreCount = static_cast<uint8>(system_info.dwNumberOfProcessors);
#endif

			GetVectorInfo(systemInfo.CPU.VectorInfo);
			GetRAMInfo(systemInfo.RAM);
			//GetCPUName(systemInfo.CPU.CpuBrand);
		}

		static Extent2D GetScreenExtent() {
			if constexpr (_WIN32) {
				int width = GetSystemMetrics(SM_CXSCREEN);
				int height = GetSystemMetrics(SM_CYSCREEN);
				return Extent2D(width, height);
			}
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
	};

	class Console
	{
	public:
		static void SetConsoleInputModeAsUTF8() {
#ifdef _WIN32
			SetConsoleOutputCP(CP_UTF8);
#endif
		}

		static void Print(const Range<const char8_t*> text) {
#ifdef _WIN32
			WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), text.GetData(), text.GetBytes() - 1, nullptr, nullptr);
#endif
		}

		//static void Read(Range<char8_t*> buffer) {
		//	DWORD characters_read{ 0 };
		//	ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), buffer.begin(), buffer.ElementCount(), &characters_read, nullptr);
		//}

		enum class ConsoleTextColor : uint8 {
			WHITE, RED, YELLOW, GREEN, ORANGE, PURPLE
		};
		static void SetTextColor(ConsoleTextColor textColor) {
#ifdef _WIN32
			WORD color{ 0 };

			switch (textColor) {
			case ConsoleTextColor::WHITE:	color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
			case ConsoleTextColor::RED:		color = FOREGROUND_INTENSITY | FOREGROUND_RED; break;
			case ConsoleTextColor::YELLOW:	color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN; break;
			case ConsoleTextColor::GREEN:	color = FOREGROUND_INTENSITY | FOREGROUND_GREEN; break;
			case ConsoleTextColor::ORANGE:	color = FOREGROUND_RED | FOREGROUND_GREEN; break;
			case ConsoleTextColor::PURPLE:	color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE; break;
			default:						break;
			}

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
#endif
		}
	};
}
