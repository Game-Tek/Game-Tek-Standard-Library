#pragma once

#include "Core.hpp"

#if (_WIN32)
#include <Windows.h>
#endif

namespace GTSL
{
	template<class _Integral, class _Ty>
	[[nodiscard]] volatile _Integral* AtomicAddressAs(_Ty& _Source) noexcept
	{
		return &reinterpret_cast<volatile _Integral>(_Source);
	}

	template <class _Integral, class _Ty>
	[[nodiscard]] _Integral* AtomicAddressAsNonVolatile(_Ty& _Source) noexcept
	{
		return &reinterpret_cast<_Integral&>(_Source);
	}

	template<typename T>
	class Atomic;

	template<>
	class alignas(64) Atomic<uint32>
	{
	public:
		using type = uint32;

		Atomic() = default;

		explicit Atomic(const type value) : value(value) {}

		type operator++()
		{
#if(_WIN32)
			return static_cast<type>(_InterlockedIncrement(AtomicAddressAs<long>(value)));
#elif __linux__
			return __atomic_add_fetch(AtomicAddressAs<long>(value), 1, __ATOMIC_SEQ_CST);
#endif
		}

		type operator--()
		{
#if(_WIN32)
			return static_cast<type>(_InterlockedDecrement(AtomicAddressAs<long>(value)));
#elif __linux__
			return __atomic_sub_fetch(AtomicAddressAs<long>(value), 1, __ATOMIC_SEQ_CST);
#endif
		}

		type operator++(int32)
		{
#if(_WIN32)
			const auto ret = value;
			static_cast<type>(_InterlockedIncrement(AtomicAddressAs<long>(value)));
			return ret;
#elif __linux__
			return __atomic_add_fetch(AtomicAddressAs<long>(value), 1, __ATOMIC_SEQ_CST);
#endif
		}

		type operator--(int32)
		{
#if(_WIN32)
			const auto ret = value;
			static_cast<type>(_InterlockedDecrement(AtomicAddressAs<long>(value)));
			return ret;
#elif __linux__
			return __atomic_sub_fetch(AtomicAddressAs<long>(value), 1, __ATOMIC_SEQ_CST);
#endif
		}

		operator type() const
		{
#if _WIN32
			_mm_mfence();
			return value;
#elif __linux__
			alignas(64) type n = 0;
			__atomic_load(AtomicAddressAs<const unsigned int>(value), AtomicAddressAsNonVolatile<unsigned int>(n), __ATOMIC_SEQ_CST);
			return n;
#endif
		}
	private:
		type value = 0;
	};

	template<>
	class alignas(64) Atomic<uint64>
	{
	public:
		using type = uint64;

		Atomic() = default;

		explicit Atomic(const type value) : value(value) {}

		type operator++()
		{
#if(_WIN32)
			return static_cast<type>(_InterlockedIncrement64(AtomicAddressAs<int64>(value)));
#elif __linux__
			return __atomic_add_fetch(AtomicAddressAs<unsigned long long>(value), 1, __ATOMIC_SEQ_CST);
#endif
		}

		type operator--()
		{
#if(_WIN32)
			return static_cast<type>(_InterlockedDecrement64(AtomicAddressAs<int64>(value)));
#elif __linux__
			return __atomic_sub_fetch(AtomicAddressAs<unsigned long long>(value), 1, __ATOMIC_SEQ_CST);
#endif
		}

		operator type() const
		{
#if _WIN32
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