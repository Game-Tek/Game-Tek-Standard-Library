#pragma once

#include "Result.h"
#include "GTSL/Core.h"
#include "GTSL/Memory.h"
#include "GTSL/Pair.hpp"

namespace GTSL {
	template<typename... ARGS>
	void EmplaceBack(std::integral auto* length, auto* array, ARGS&&... args) {
		::new(array+ *length) std::decay_t<decltype(*array)>(GTSL::ForwardRef<ARGS>(args)...);
		++(*length);
	}

	template<typename T>
	void PopElement(const uint64 capacity, std::integral auto length, T* array, uint64 index) {
		auto elementsToMove = length - index - 1;

		Destroy(array[index]);

		for (uint64 i = 0; i < elementsToMove; ++i) {
			Move(array + index + i + 1, array + index + i);
		}
	}

	template<typename T>
	void PopElement(const uint64 capacity, std::integral auto* length, T* array, uint64 index) {
		PopElement(capacity, *length, array, index);
		--(*length);
	}

	template<typename T, typename... ARGS>
	void InsertElement(const uint64 capacity, std::integral auto length, T* array, std::integral auto index, ARGS&&... args) {
		auto elementsToMove = length - GTSL::Math::Min(index, length);

		for(uint64 i = 0, t = length - 1; i < elementsToMove; ++i, --t) {
			Move(array + t, array + t + 1);
		}

		::new(array + index) T(ForwardRef<ARGS>(args)...);
	}

	template<typename T, typename... ARGS>
	void InsertElement(const uint64 capacity, std::integral auto* length, T* array, uint64 index, ARGS&&... args) {
		InsertElement(capacity, *length, array, index, GTSL::ForwardRef<ARGS>(args)...);
		++(*length);
	}
}
