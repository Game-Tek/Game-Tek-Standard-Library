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
	
	template <class _Integral, class _Ty>
	[[nodiscard]] _Integral* AtomicAddressAsNonVolatile(_Ty& _Source) noexcept {
		// gets a pointer to the argument as an integral type (to pass to intrinsics)
		return &reinterpret_cast<_Integral&>(_Source);
	}

	template<typename T>
	class Atomic;

	template<>
	class Atomic<uint32> {
	public:
		using type = uint32;
		
		Atomic() = default;

		explicit Atomic(const type value) : value(value) {}

		type operator++() {
#if(_WIN64)
			return static_cast<type>(_InterlockedIncrement(AtomicAddressAs<long>(value)));
#elif __linux__
			return __atomic_add_fetch(AtomicAddressAs<long>(value), 1, __ATOMIC_SEQ_CST);
#endif
		}

		type operator--() {
#if(_WIN64)
			return static_cast<type>(_InterlockedDecrement(AtomicAddressAs<long>(value)));
#elif __linux__
			return __atomic_sub_fetch(AtomicAddressAs<long>(value), 1, __ATOMIC_SEQ_CST);
#endif
		}

		type operator++(int32) {
#if(_WIN64)
			const auto ret = value;
			static_cast<type>(_InterlockedIncrement(AtomicAddressAs<long>(value)));
			return ret;
#elif __linux__
			return __atomic_add_fetch(AtomicAddressAs<long>(value), 1, __ATOMIC_SEQ_CST);
#endif
		}

		type operator--(int32) {
#if(_WIN64)
			const auto ret = value;
			static_cast<type>(_InterlockedDecrement(AtomicAddressAs<long>(value)));
			return ret;
#elif __linux__
			return __atomic_sub_fetch(AtomicAddressAs<long>(value), 1, __ATOMIC_SEQ_CST);
#endif
		}

		operator type() const {
#if _WIN64
			_mm_mfence();
			return value;
#elif __linux__
			alignas(64) type n = 0;
			__atomic_load(AtomicAddressAs<const unsigned long>(value), AtomicAddressAsNonVolatile<unsigned long>(n), __ATOMIC_SEQ_CST);
			return n;
#endif
		}
	private:
		alignas(type) type value = 0;
	};

	template<>
	class Atomic<uint64>
	{
	public:
		using type = uint64;

		Atomic() = default;
		
		explicit Atomic(const type value) : value(value) {}

		type operator++() {
#if(_WIN64)
			return static_cast<type>(_InterlockedIncrement64(AtomicAddressAs<int64>(value)));
#elif __linux__
			return __atomic_add_fetch(AtomicAddressAs<unsigned long long>(value), 1, __ATOMIC_SEQ_CST);
#endif
		}

		type operator--() {
#if(_WIN64)
			return static_cast<type>(_InterlockedDecrement64(AtomicAddressAs<int64>(value)));
#elif __linux__
			return __atomic_sub_fetch(AtomicAddressAs<unsigned long long>(value), 1, __ATOMIC_SEQ_CST);
#endif
		}
		
		operator type() const {
#if _WIN64
			_mm_mfence();
			return value;
#elif __linux__
			alignas(64) type n = 0;
			__atomic_load(AtomicAddressAs<const unsigned long long>(value), AtomicAddressAsNonVolatile<unsigned long long>(n), __ATOMIC_SEQ_CST);
			return n;
#endif
		}
	private:
		type value = 0;
	};
}
