#pragma once
#include "Assert.h"
#include "Bitman.h"
#include "Core.h"
#include "Result.h"
#include "Algorithm.h"

namespace GTSL
{
	template<Integral T>
	uint32 GetAllocationSize(const uint32 numElementToBeSupported)
	{
		return ((numElementToBeSupported / Bits<T>()) + 1) * sizeof(T);
	}
	
	template<Integral T>
	void InitializeBits(T* bits, const uint32 numElements)
	{
		for (uint32 i = 0; i < (numElements / Bits<T>()) + 1; ++i) { bits[i] = 0xFFFFFFFFFFFFFFFF; }
	}

	template<Integral T>
	void SetAsOccupied(T* bits, const uint32 num, const uint32 pos)
	{
		GTSL_ASSERT(CheckBit(pos % Bits<T>(), bits[pos / Bits<T>()]), "Slot is already occupied")
		GTSL::ClearBit(pos % Bits<T>(), bits[pos / Bits<T>()]);
	}

	template<Integral T>
	void SetAsFree(T* bits, const uint32 num, const uint32 pos)
	{
		GTSL_ASSERT(!CheckBit(pos % Bits<T>(), bits[pos / Bits<T>()]), "Slot is not occupied")
		GTSL::SetBit(pos % Bits<T>(), bits[pos / Bits<T>()]);
	}

	template<Integral T>
	Result<uint32> OccupyFirstFreeSlot(T* bits, const uint32 numElementSupported)
	{
		for (uint32 i = 0; i < (numElementSupported / Bits<T>()) + 1; ++i)
		{
			uint8 bit; bool any;
			FindFirstSetBit(bits[i], bit, any);
			if (any)
			{
				GTSL::ClearBit(bit, bits[i]);
				return Result<uint32>(i * Bits<T>() + bit, true);
			}
		}

		return Result<uint32>(false);
	}
	
	template<Integral T>
	Result<uint32> FindFirstFreeSlot(const T* bits, const uint32 numElements)
	{
		for(uint32 i = 0; i < (numElements / Bits<T>()) + 1; ++i)
		{
			uint8 bit; bool any;
			FindFirstSetBit(bits[i], bit, any);
			if (any) { return Result<uint32>(i * Bits<T>() + bit, true); }
		}

		return Result<uint32>(false);
	}
}
