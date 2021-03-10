#include "GTSL/StringCommon.h"

#include <cstdio>

#include "GTSL/StaticString.hpp"
#include "GTSL/Math/Math.hpp"

void GTSL::ToString(const int8 num, Range<UTF8*>& buffer)
{
	buffer = Range<UTF8*>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%c", num) + 1, buffer.begin());
}

void GTSL::ToString(const uint8 num, Range<UTF8*>& buffer)
{
	buffer = Range<UTF8*>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%c", num) + 1, buffer.begin());
}

void GTSL::ToString(const int16 num, Range<UTF8*>& buffer)
{
	buffer = Range<UTF8*>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%hi", num) + 1, buffer.begin());
}

void GTSL::ToString(const uint16 num, Range<UTF8*>& buffer)
{
	buffer = Range<UTF8*>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%hu", num) + 1, buffer.begin());
}

void GTSL::ToString(const int32 num, Range<UTF8*>& buffer)
{
	buffer = Range<UTF8*>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%d", num) + 1, buffer.begin());
}

void GTSL::ToString(const uint32 num, Range<UTF8*>& buffer)
{
	buffer = Range<UTF8*>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%u", num) + 1, buffer.begin());
}

void GTSL::ToString(const int64 num, Range<UTF8*>& buffer)
{
	buffer = Range<UTF8*>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%lld", num) + 1, buffer.begin());
}

void GTSL::ToString(const uint64 num, Range<UTF8*>& buffer)
{
	buffer = Range<UTF8*>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%llu", num) + 1, buffer.begin());
}

void GTSL::ToString(const float32 num, Range<UTF8*>& buffer)
{
	buffer = Range<UTF8*>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%f", num) + 1, buffer.begin());
}

void GTSL::ToString(const float64 num, Range<UTF8*>& buffer)
{
	buffer = Range<UTF8*>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%lf", num) + 1, buffer.begin());
}

void GTSL::ToString(uint32 num, StaticString<32>& string)
{
	uint8 i = 30, len = 0;

	UTF8 str[32]; str[31] = '\0';

	// Process individual digits 
	while (num != 0) {
		//uint8 rem = num % 10;
		str[i--] = (num % 10) + '0';
		num /= 10;
		++len;
	}

	string += GTSL::Range<const UTF8*>(len + 1, str + i + 1);
}

void GTSL::ToString(int32 num, StaticString<32>& string)
{
	uint8 i = 30, len = 0;

	UTF8 str[32]; str[31] = '\0';

	// Process individual digits 
	while (num != 0) {
		//uint8 rem = num % 10;
		str[i--] = (num % 10) + '0';
		num /= 10;
		++len;
	}

	if (num < 0) { str[i--] = '-'; ++len; }

	string += GTSL::Range<const UTF8*>(len + 1, str + i + 1);
}

void GTSL::ToString(float32 num, StaticString<32>& string)
{
	uint8 i = 30, len = 0;
	UTF8 str[32]; str[31] = '\0'; bool isNegative = false;

	if (num < 0.0f) { num = -num; isNegative = true; }
	
	// Extract integer part 
	int32 ipart = (int32)num;

	// Extract floating part 
	float32 fpart = num - (float32)ipart;

	auto intToStr = [&](int32 integer)
	{
		while (integer) {
			str[i--] = (integer % 10) + '0';
			integer /= 10;
			++len;
		}
	};

	// check for display option after point 
	if (6 != 0) {
		// Get the value of fraction part upto given no. 
		// of points after dot. The third parameter  
		// is needed to handle cases like 233.007 
		fpart = fpart * Math::Power(10, 6);

		intToStr((int32)fpart);

		str[i--] = '.'; // add dot
		++len;
	}

	// convert integer part to string 
	intToStr(ipart);

	if (isNegative) { str[i--] = '-'; ++len; }
	
	string += GTSL::Range<const UTF8*>(len + 1, str + i + 1);
}
