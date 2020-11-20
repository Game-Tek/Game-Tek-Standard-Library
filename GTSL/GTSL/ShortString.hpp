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
		constexpr ShortString()
		{
			static_assert(SIZE < 255, "Size must be less that 255");
			array[SIZE - 1] = 32;
		}

		operator Range<UTF8*>() { return Range<UTF8*>(GetLength(), array); }
		operator Range<const UTF8*>() const { return Range<const UTF8*>(GetLength(), array); }

		constexpr ShortString(const UTF8* text)
		{
			array[SIZE - 1] = 32;
			(*this) += Range<const UTF8*>(StringLength(text), text);
		}

		constexpr ShortString& operator+=(const Range<const UTF8*> text)
		{
			const auto toCopy = GetLength() + text.ElementCount() <= array[SIZE - 1] ? text.ElementCount() : array[SIZE - 1];
			for (uint8 s = GetLength(), i = 0; i < toCopy; ++i, ++s) { array[s] = text[i]; }
			array[SIZE - 1] -= toCopy;
			return *this;
		}
		
		//WITH NULL
		[[nodiscard]] constexpr uint8 GetLength() const { return SIZE - array[SIZE - 1]; }
		
	private:
		UTF8 array[SIZE]{ 0 };
	};
}