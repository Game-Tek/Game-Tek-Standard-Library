//module;

#pragma once

#include "Core.h"
#include "Range.h"
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
		constexpr ShortString(const char(&string)[N]) : ShortString() {
			(*this) += string;
		}
		
		constexpr operator Range<UTF8*>() { return Range<UTF8*>(GetLength(), array); }
		constexpr operator Range<const UTF8*>() const { return Range<const UTF8*>(GetLength(), array); }

		[[nodiscard]] constexpr const UTF8* begin() const { return array; }
		[[nodiscard]] constexpr const UTF8* end() const { return array + GetLength(); }
		
		constexpr ShortString(const UTF8* text) : ShortString() {
			(*this) += Range<const UTF8*>(StringLength(text), text);
		}

		constexpr ShortString(const Range<const UTF8*> text) : ShortString() {
			(*this) += text;
		}

		constexpr ShortString& operator=(const Range<const UTF8*> range) {
			array[SIZE - 1] = SIZE;
			*this += range; return *this;
		}
		
		constexpr ShortString& operator+=(const Range<const UTF8*> text)
		{
			auto oldSize = array[SIZE - 1];
			const auto toCopy = GetLength() + text.ElementCount() <= array[SIZE - 1] ? text.ElementCount() : oldSize;
			for (uint8 s = GetLength(), i = 0; i < toCopy; ++i, ++s) { array[s] = text[i]; }
			array[SIZE - 1] = static_cast<uint8>(oldSize - toCopy);
			return *this;
		}

		template<uint8 N>
		constexpr bool operator==(const ShortString<N>& other) const {
			if (GetLength() != other.GetLength()) { return false; }
			for (uint8 i = 0; i < GetLength(); ++i) { if (array[i] != other.array[i]) { return false; } }
			return true;
		}
		
		//WITH NULL
		[[nodiscard]] constexpr uint8 GetLength() const { return SIZE - array[SIZE - 1]; }
		
	private:
		UTF8 array[SIZE]{ 0 };

		friend class ShortString;
	};
}