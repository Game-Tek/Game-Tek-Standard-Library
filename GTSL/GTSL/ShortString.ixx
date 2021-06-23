module;

#include "Core.h"

export module ShortString;

namespace GTSL
{
	template<uint8 SIZE>
	export class ShortString
	{
	public:
		ShortString()
		{
			static_assert(SIZE < 255, "Size must be less that 255");
		}

		operator Range<const UTF8*> const { return Range<const UTF8*>(array[SIZE - 1], array); }

		ShortString(const UTF8* text)
		{
			array[SIZE - 1] = StringLength(text);
			for (uint8 i = 0; i < array[SIZE - 1]; ++i) { array[i] = text[i]; }
		}

	private:
		uint8 array[SIZE];
	};
}