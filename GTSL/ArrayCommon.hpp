#pragma once

#include "Result.h"
#include "GTSL/Core.h"
#include "GTSL/Memory.h"
#include "GTSL/Pair.h"

namespace GTSL
{
	template<typename T>
	void PopElement(const uint64 capacity, const uint64 length, T* array, uint64 index) {
		auto elementsToMove = length - index - 1;

		Destroy(array[index]);

		for (uint64 i = 0; i < elementsToMove; ++i) {
			Move(array + index + i + 1, array + index + i);
		}
	}

	template<typename T, typename... ARGS>
	void InsertElement(const uint64 capacity, const uint64 length, T* array, uint64 index, ARGS&&... args) {
		auto elementsToMove = length - index;

		for(uint64 i = 0, t = length - 1; i < elementsToMove; ++i, --t) {
			Move(array + t, array + t + 1);
		}

		::new(array + index) T(ForwardRef<ARGS>(args)...);
	}
}
