#pragma once

#include "Core.h"
#include <intrin.h>

namespace GTSL
{
	template<typename T>
	class Atomic;

	template<>
	class Atomic<uint32>
	{
	public:
		using type = uint32;
		
		explicit Atomic(const type value) : value(value) {}

		type operator++() { return static_cast<type>(_InterlockedIncrement(reinterpret_cast<long*>(&value))); }
		type operator--() { return static_cast<type>(_InterlockedDecrement(reinterpret_cast<long*>(&value))); }

		operator type() const
		{
			_mm_mfence();
			return value;
		}
	private:
		type value{ 0 };
	};

	template<>
	class Atomic<uint64>
	{
	public:
		using type = uint64;
		
		explicit Atomic(const type value) : value(value) {}

		type operator++() { return static_cast<type>(_InterlockedIncrement64(reinterpret_cast<int64*>(&value))); }
		type operator--() { return static_cast<type>(_InterlockedDecrement64(reinterpret_cast<int64*>(&value))); }
		
		operator type() const
		{
			_mm_mfence();
			return value;
		}
	private:
		type value{ 0 };
	};
}
