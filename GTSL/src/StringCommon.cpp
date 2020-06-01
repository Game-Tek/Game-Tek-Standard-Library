#include "GTSL/StringCommon.h"

#include <cstdio>

void GTSL::ToString(const int8 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%c", num), buffer.begin());
}

void GTSL::ToString(const uint8 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%c", num), buffer.begin());
}

void GTSL::ToString(const int16 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%hi", num), buffer.begin());
}

void GTSL::ToString(const uint16 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%hu", num), buffer.begin());
}

void GTSL::ToString(const int32 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%d", num), buffer.begin());
}

void GTSL::ToString(const uint32 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%u", num), buffer.begin());
}

void GTSL::ToString(const int64 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%lld", num), buffer.begin());
}

void GTSL::ToString(const uint64 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%llu", num), buffer.begin());
}

void GTSL::ToString(const float32 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%f", num), buffer.begin());
}

void GTSL::ToString(const float64 num, Ranger<UTF8>& buffer)
{
	buffer = Ranger<UTF8>(sprintf_s(buffer.begin(), buffer.end() - buffer.begin(), "%lf", num), buffer.begin());
}
