#pragma once

#include "Core.h"
#include "Assert.h"

namespace GTSL
{
	template<typename I>
	struct Range
	{
		Range(I b, I e) : from(b), to(e) {}

		I& begin() { return from; }
		I& end() { return to; }

		const I& begin() const { return from; }
		const I& end() const { return to; }

		Range Reverse() const { return Range(end(), begin()); }
		
		using type = I;
		
	private:
		I from, to;

		friend struct Range;
	};

	template<typename I>
	struct Range<I*>
	{
		Range() = default;
		
		constexpr Range(I* b, I* e) : from(b), to(e) {}

		constexpr Range(const uint64 length, I* b) : from(b), to(b + length) {}

		[[nodiscard]] constexpr I* begin() { return from; }
		[[nodiscard]] constexpr I* end() { return to; }
		
		//[[nodiscard]] I* begin() const { return from; }
		//[[nodiscard]] I* end() const { return to; }

		[[nodiscard]] Range Reverse() const { return Range(end() - 1, begin() - 1); }	
		
		[[nodiscard]] constexpr I* begin() const { return from; }
		[[nodiscard]] constexpr I* end() const { return to; }

		using type = I;

		[[nodiscard]] constexpr uint64 Bytes() const { return (to - from) * sizeof(I); }

		[[nodiscard]] constexpr uint64 ElementCount() const noexcept { return to - from; }

		constexpr I& operator[](const uint64 i)
		{
			GTSL_ASSERT((this->from + i) < to, "Unbounded access!")
			return this->from[i];
		}

		constexpr I& operator[](const uint64 i) const
		{
			GTSL_ASSERT((this->from + i) < to, "Unbounded access!")
			return this->from[i];
		}

		operator Range<const I*>() const { return Range<const I*>(static_cast<const I*>(this->from), static_cast<const I*>(this->to)); }
		
	private:
		I* from = nullptr,* to = nullptr;

		friend struct Range;
	};
	
	//template<typename T>
	//class Ranger
	//{
	//public:
	//	using type = T;
	//	using type_pointer = T*;
	//
	//	Ranger() = default;
	//
	//	Ranger(const Ranger& other) noexcept : from(other.from), to(other.to) {}
	//	
	//	constexpr Ranger(T* start, T* end) noexcept : from(start), to(end)
	//	{
	//	}
	//
	//	constexpr Ranger(uint64 length, T* start) noexcept : from(start), to(start + length)
	//	{
	//	}
	//
	//	//template<typename TT>
	//	//Ranger(const Ranger<TT>& other) noexcept : from(static_cast<T*>(other.begin())), to(static_cast<T*>(other.end())) {}
	//	
	//	constexpr T* begin() noexcept { return from; }
	//	constexpr T* end() noexcept { return to; }
	//	[[nodiscard]] constexpr T* begin() const noexcept { return from; }
	//	[[nodiscard]] constexpr T* end() const noexcept { return to; }
	//
	//	[[nodiscard]] constexpr uint64 Bytes() const noexcept { return ((to - from) * sizeof(type)); }
	//
	//	[[nodiscard]] constexpr uint64 ElementCount() const { return to - from; }
	//
	//	//T* operator+(const uint64 i) const { return this->from + i; }
	//	T* operator*(const uint64 i) const { return this->from * i; }
	//	T* operator/(const uint64 i) const { return this->from / i; }
	//
	//	operator T*() { return this->from; }
	//	operator T*() const { return this->from; }
	//	
	//	T& operator[](const uint64 i)
	//	{
	//		GTSL_ASSERT((this->from + i) < to, "Unbounded access!")
	//		return this->from[i];
	//	}
	//	//T& operator[](const uint64 i) const { return this->from[i]; }
	//
	//	operator Ranger<const T>() const { return Ranger<const T>(static_cast<const T*>(this->from), static_cast<const T*>(this->to)); }
	//
	//protected:
	//	T* from = nullptr, * to = nullptr;
	//	friend class Ranger;
	//};
}
