#pragma once

#include "../Result.hpp"
#include "../Core.hpp"
#include "../Memory.hpp"

namespace GTSL
{
	template<typename... Args>
	void EmplaceBack(std::integral auto* length, auto* arr, Args&&... args)
	{
		::new(arr + *length) std::decay_t<decltype(*arr)>(GTSL::ForwardRef<Args>(args)...);
	}

	template<typename T>
	void PopElement(const uint64 capacity, std::integral auto length, T* arr, uint64 index)
	{
		auto elementsToMove = length - index - 1;
		Destroy(arr[index]);

		for(uint64 i = 0; i < elementsToMove; ++i)
		{
			Move(arr + index + i + 1, arr + index + 1);
		}
	}

	template<typename T>
	void PopElement(const uint64 capacity, std::integral auto* length, T* arr, uint64 index)
	{
		PopElement(capacity, *length, arr, index);
		--(*length);
	}

	template<typename T, typename... ARGS>
	void InsertElement(const uint64 capacity, std::integral auto length, T* arr, std::integral auto index, ARGS&&... args)
	{
		auto elementsToMove = length - GTSL::Math::Min(index, length);

		for (uint64 i = 0, t = length - 1; i < elementsToMove; ++i, --t) {
			Move(arr + t, arr + t + 1);
		}

		::new(arr + index) T(ForwardRef<ARGS>(args)...);
	}

	template<typename T, typename... ARGS>
	void InsertElement(const uint64 capacity, std::integral auto* length, T* arr, uint64 index, ARGS&&... args)
	{
		InsertElement(capacity, *length, arr, index, GTSL::ForwardRef<ARGS>(args)...);
		++(*length);
	}
}