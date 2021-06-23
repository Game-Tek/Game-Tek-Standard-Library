#pragma once
#include "Assert.h"
#include "Bitman.h"
#include "Core.h"
#include "Result.h"
#include "Algorithm.h"
#include "Range.h"

namespace GTSL
{
	template<Integral T>
	uint32 GetAllocationSize(const uint32 numElementToBeSupported) {
		return ((numElementToBeSupported / Bits<T>()) + 1) * sizeof(T);
	}
	
	template<Integral T>
	void InitializeBits(Range<T*> bits) {
		for (auto& e : bits) { e = 0; }
	}

	template<Integral T>
	void SetAsOccupied(Range<T*> bits, const uint32 pos) {
		GTSL_ASSERT(!CheckBit(pos % Bits<T>(), bits[pos / Bits<T>()]), "Slot is already occupied")
		GTSL::SetBit(pos % Bits<T>(), bits[pos / Bits<T>()]);
	}

	template<Integral T>
	void SetAsFree(Range<T*> bits, const uint32 pos) {
		GTSL_ASSERT(CheckBit(pos % Bits<T>(), bits[pos / Bits<T>()]), "Slot is not occupied")
		GTSL::ClearBit(pos % Bits<T>(), bits[pos / Bits<T>()]);
	}

	template<Integral T>
	Result<uint32> OccupyFirstFreeSlot(Range<T*> bits, uint32 maxLength) {
		for (uint32 i = 0, iBits = 0; i < bits.ElementCount(); ++i, iBits += Bits<T>()) {
			if (auto bit = FindFirstClearBit(bits[i]); bit) {
				if (bit.Get() < maxLength) {
					GTSL::SetBit(bit.Get(), bits[i]);
					return Result<uint32>(iBits + bit.Get(), true);
				}

				break;
			}
		}

		return Result<uint32>(false);
	}

	template<Integral T>
	bool IsSlotOccupied(Range<const T*> bits, uint32 pos) {
		return GTSL::CheckBit(pos % Bits<T>(), bits[pos / Bits<T>()]);
	}
}
