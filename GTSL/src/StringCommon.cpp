#include "GTSL/StringCommon.h"

#include <cstdio>

void GTSL::ToString(const int8 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%c", num) + 1, buffer.begin());
}

void GTSL::ToString(const uint8 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%c", num) + 1, buffer.begin());
}

void GTSL::ToString(const int16 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%hi", num) + 1, buffer.begin());
}

void GTSL::ToString(const uint16 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%hu", num) + 1, buffer.begin());
}

void GTSL::ToString(const int32 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%d", num) + 1, buffer.begin());
}

void GTSL::ToString(const uint32 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%u", num) + 1, buffer.begin());
}

void GTSL::ToString(const int64 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%lld", num) + 1, buffer.begin());
}

void GTSL::ToString(const uint64 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%llu", num) + 1, buffer.begin());
}

void GTSL::ToString(const float32 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%f", num) + 1, buffer.begin());
}

void GTSL::ToString(const float64 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%lf", num) + 1, buffer.begin());
}
