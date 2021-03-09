#pragma once
#include "Core.h"
#include "DataSizes.h"
#include "Extent.h"

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
		static void GetRAMInfo(RamInfo& ramInfo);
		static void GetVectorInfo(VectorInfo& vectorInfo);
		static void GetSystemInfo(SystemInfo& systemInfo);
		static Extent2D GetScreenExtent();
	};
}
