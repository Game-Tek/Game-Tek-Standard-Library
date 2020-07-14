#pragma once

#include "Core.h"

#if(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

namespace GTSL
{
	template <class _Integral, class _Ty>
	[[nodiscard]] volatile _Integral* AtomicAddressAs(_Ty& _Source) noexcept {
		// gets a pointer to the argument as an integral type (to pass to intrinsics)
		return &reinterpret_cast<volatile _Integral&>(_Source);
	}
	
	template<typename T>
	class Atomic;

	template<>
	class Atomic<uint32>
	{
	public:
		using type = uint32;
		
		explicit Atomic(const type value) : value(value) {}

		type operator++() { return static_cast<type>(_InterlockedIncrement(AtomicAddressAs<long>(value))); }
		type operator--() { return static_cast<type>(_InterlockedDecrement(AtomicAddressAs<long>(value))); }
		type operator++(int32) { const auto ret = value; static_cast<type>(_InterlockedIncrement(AtomicAddressAs<long>(value))); return ret; }
		type operator--(int32) { const auto ret = value; static_cast<type>(_InterlockedDecrement(AtomicAddressAs<long>(value))); return ret; }

		operator type() const
		{
			_mm_mfence();
			return value;
		}
	private:
		alignas(type) type value{ 0 };
	};

	template<>
	class Atomic<uint64>
	{
	public:
		using type = uint64;
		
		explicit Atomic(const type value) : value(value) {}

		type operator++() { return static_cast<type>(_InterlockedIncrement64(AtomicAddressAs<int64>(value))); }
		type operator--() { return static_cast<type>(_InterlockedDecrement64(AtomicAddressAs<int64>(value))); }
		
		operator type() const
		{
			_mm_mfence();
			return value;
		}
	private:
		type value{ 0 };
	};
}
