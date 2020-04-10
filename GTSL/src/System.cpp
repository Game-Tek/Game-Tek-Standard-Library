#include "System.h"

#if (_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "intrin.h"
#endif

void GTSL::System::GetRunningPath(GTSL::String& path)
{
#if (_WIN32)
	char a[512];
	GetModuleFileNameA(NULL, a, 512);
	//GTSL_ASSERT(GetModuleFileNameA(NULL, a, 512), "Failed to get Win32 module file name!")
	path += a;
	path.Drop(path.FindLast('\\') + 1);
	path.ReplaceAll('\\', '/');
#endif
}

void GTSL::System::GetRAMInfo(RamInfo& ramInfo)
{
#if (_WIN32)
	MEMORYSTATUSEX memory_status{};
	GlobalMemoryStatusEx(&memory_status);

	//GTSL_ASSERT(memory_status, "Failed to get Win32 memory status!");

	ramInfo.FreePhysicalMemory = memory_status.ullAvailPhys;
	ramInfo.TotalPhysicalMemory = memory_status.ullTotalPhys;
	ramInfo.ProcessAvailableMemory = memory_status.ullAvailPageFile;
#endif
}

void GTSL::System::GetVectorInfo(VectorInfo& vectorInfo)
{
#if (_WIN32)
	//https://stackoverflow.com/questions/6121792/how-to-check-if-a-cpu-supports-the-sse3-instruction-set

	//bool OS_x64;
	//bool OS_AVX;
	//bool OS_AVX512;

	int info[4];
	__cpuidex(info, 0, 0);
	int nIds = info[0];

	__cpuidex(info, 0x8000000, 0);
	uint32 nExIds = info[0];

	//  Detect Features
	if (nIds >= 0x00000001)
	{
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
	if (nIds >= 0x00000007)
	{
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
	if (nExIds >= 0x80000001)
	{
		__cpuidex(info, 0x80000001, 0);
		vectorInfo.HW_x64 = (info[3] & (static_cast<int>(1) << 29)) != 0;
		vectorInfo.HW_ABM = (info[2] & (static_cast<int>(1) << 5)) != 0;
		vectorInfo.HW_SSE4a = (info[2] & (static_cast<int>(1) << 6)) != 0;
		vectorInfo.HW_FMA4 = (info[2] & (static_cast<int>(1) << 16)) != 0;
		vectorInfo.HW_XOP = (info[2] & (static_cast<int>(1) << 11)) != 0;
	}
#endif
}


void GTSL::System::GetCPUName(GTSL::String& name)
{
#if (_WIN32)
	int CPUInfo[4] = { -1 };
	unsigned i = 0;
	const unsigned nExIds = CPUInfo[0];
	char CPUBrandString[0x40];
	// Get the information associated with each extended ID.
	__cpuid(CPUInfo, 0x80000000);
	for (i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(CPUInfo, i);
		// Interpret CPU brand string
		if (i == 0x80000002)
		memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000003)
		memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000004)
		memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
	}

	name += CPUBrandString;
#endif
}

void GTSL::System::GetSystemInfo(SystemInfo& systemInfo)
{
#if (_WIN32)
	SYSTEM_INFO system_info;
	::GetSystemInfo(&system_info);

	systemInfo.CPU.CoreCount = static_cast<uint8>(system_info.dwNumberOfProcessors);
#endif
	
	GetVectorInfo(systemInfo.CPU.VectorInfo);
	GetRAMInfo(systemInfo.RAM);
	GetCPUName(systemInfo.CPU.CpuBrand);
}
