//module;

#pragma once

#include "Core.h"
#include "Range.hpp"
#include "StringCommon.h"

//export module ShortString;

namespace GTSL
{
	template<uint8 SIZE>
	/*export*/ class ShortString
	{
	public:
		static_assert(SIZE < 255, "Size must be less that 255");
		
		constexpr ShortString() {
			array[SIZE - 1] = SIZE;
		}

		template<uint8 N>
		constexpr ShortString(const ShortString<N>& other) : ShortString() {
			(*this) += other;
		}

		template<uint64 N>
		constexpr ShortString(const char8_t(&string)[N]) : ShortString() {
			(*this) += string;
		}
		
		constexpr operator Range<const char8_t*>() const { return Range<const char8_t*>(array); }

		[[nodiscard]] constexpr const char8_t* begin() const { return array; }
		[[nodiscard]] constexpr const char8_t* end() const { return array + GetLength(); }

		constexpr ShortString(const Range<const char8_t*> text) : ShortString() {
			(*this) += text;
		}

		constexpr ShortString& operator=(const Range<const char8_t*> range) {
			array[SIZE - 1] = SIZE;
			*this += range; return *this;
		}

		constexpr ShortString& operator+=(const Range<const char8_t*> text) {
			const auto toCopy = text.GetBytes() <= array[SIZE - 1] ? text.GetBytes() : array[SIZE - 1];
			for (uint16 s = GetLength(), i = 0; i < toCopy; ++i, ++s) { array[s] = text[i]; }
			array[SIZE - 1] -= toCopy;
			return *this;
		}

		template<uint8 N>
		constexpr bool operator==(const ShortString<N>& other) const {
			if (GetLength() != other.GetLength()) { return false; }
			for (uint16 i = 0; i < GetLength(); ++i) { if (array[i] != other.array[i]) { return false; } }
			return true;
		}
		
		//WITH NULL
		[[nodiscard]] constexpr uint16 GetLength() const { return SIZE - array[SIZE - 1]; }

		friend void Insert(const ShortString& string, auto& buffer) {
			buffer.Write(SIZE, reinterpret_cast<const byte*>(string.array));
		}

		friend void Extract(ShortString& string, auto& buffer) {
			buffer.Read(SIZE, reinterpret_cast<byte*>(string.array));
		}
		
	private:
		char8_t array[SIZE]{ 0 };

		friend class ShortString;
	};
}