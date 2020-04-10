#include "GTSLString.h"

#include <cstdio>
#include <cstdarg>

#include "Array.hpp"
#include "Ranger.h"
#include "Stream.h"

#include "Serialize.h"

GTSL::String::String() : data(10)
{
}

GTSL::String::String(const length_type length, const char* cstring): data(length + 1, const_cast<char*>(cstring))
{
	data.PushBack(char('\0'));
}

GTSL::String::String(const length_type length, const String& string): data(length, string.data.GetData())
{
	data.PushBack(char('\0'));
}

GTSL::String::String(const length_type length, const String& string, const length_type offset): data(length, string.data.GetData() + offset)
{
	data.PushBack(char('\0'));
}

GTSL::String& GTSL::String::operator=(const char* cstring)
{
	data.Recreate(StringLength(cstring), const_cast<char*>(cstring));
	return *this;
}

GTSL::String& GTSL::String::operator+=(char c)
{
	data.PopBack();
	data.PushBack(c);
	data.PushBack('\0');
	return *this;
}

GTSL::String& GTSL::String::operator+=(const char* cstring)
{
	data.PopBack();
	data.PushBack(StringLength(cstring), cstring);
	return *this;
}

GTSL::String& GTSL::String::operator+=(const String& string)
{
	data.PopBack(); data.PushBack(string.data); return *this;
}

bool GTSL::String::operator==(const String& other) const
{
	//Discard if Length of strings is not equal, first because it helps us discard before even starting, second because we can't compare strings of different sizes.
	if (data.GetLength() != other.data.GetLength()) return false;

	length_type i = 0;
	for (const auto& c : data) { if (c != other.data[i]) { return false; } ++i; }
	return true;
}

bool GTSL::String::NonSensitiveComp(const String& other) const
{
	//Discard if Length of strings is not equal, first because it helps us discard before even starting, second because we can't compare strings of different sizes.
	if (data.GetLength() != other.data.GetLength()) return false;

	length_type i = 0;
	for (const auto& c : data) { if (c != (toLowerCase(other.data[i]) || toUpperCase(other.data[i]))) { return false; } ++i; }

	return true;
}

void GTSL::String::Append(const char* cstring)
{
	data.PopBack(); //Get rid of null terminator.
	data.PushBack(' '); //Push space.
	data.PushBack(StringLength(cstring), const_cast<char*>(cstring));
	return;
}

void GTSL::String::Append(const String& string)
{
	data.PopBack(); //Get rid of null terminator.
	data.PushBack(' '); //Push space.
	data.PushBack(string.data); //Push new string.
	return;
}

void GTSL::String::Append(const uint8 number)
{
	data.PopBack();
	data.PushBack(' ');
	data.Resize(data.GetLength() + 3 + 1);
	data.Resize(sprintf_s(data.GetData() + data.GetLength() - 1, data.GetCapacity() - data.GetLength() - 1, "%d", number) + 1);
}

void GTSL::String::Append(const int8 number)
{
	data.PopBack();
	data.PushBack(' ');
	data.Resize(data.GetLength() + 4 + 1);
	data.Resize(sprintf_s(data.GetData() + data.GetLength() - 1, data.GetCapacity() - data.GetLength() - 1, "%d", number) + 1);
}

void GTSL::String::Append(const uint16 number)
{
	data.PopBack();
	data.PushBack(' ');
	data.Resize(data.GetLength() + 6 + 1);
	data.Resize(sprintf_s(data.GetData() + data.GetLength() - 1, data.GetCapacity() - data.GetLength() - 1, "%hu", number) + 1);
}

void GTSL::String::Append(const int16 number)
{
	data.PopBack();
	data.PushBack(' ');
	data.Resize(data.GetLength() + 7 + 1);
	data.Resize(sprintf_s(data.GetData() + data.GetLength() - 1, data.GetCapacity() - data.GetLength() - 1, "%hi", number) + 1);
}

void GTSL::String::Append(const uint32 number)
{
	data.PopBack();
	data.PushBack(' ');
	data.Resize(data.GetLength() + 10 + 1);
	data.Resize(sprintf_s(data.GetData() + data.GetLength() - 1, data.GetCapacity() - data.GetLength() - 1, "%lu", number) + 1);
}

void GTSL::String::Append(const int32 number)
{
	data.PopBack();
	data.PushBack(' ');
	data.Resize(data.GetLength() + 11 + 1);
	data.Resize(sprintf_s(data.GetData() + data.GetLength() - 1, data.GetCapacity() - data.GetLength() - 1, "%d", number) + 1);
}

void GTSL::String::Append(const uint64 number)
{
	data.PopBack();
	data.PushBack(' ');
	data.Resize(data.GetLength() + 20 + 1);
	data.Resize(sprintf_s(data.GetData() + data.GetLength() - 1, data.GetCapacity() - data.GetLength() - 1, "%llu", number) + 1);
}

void GTSL::String::Append(const int64 number)
{
	data.PopBack();
	data.PushBack(' ');
	data.Resize(data.GetLength() + 21 + 1);
	data.Resize(sprintf_s(data.GetData() + data.GetLength() - 1, data.GetCapacity() - data.GetLength() - 1, "%lld", number) + 1);
}

void GTSL::String::Append(const float number)
{
	data.PopBack();
	data.PushBack(' ');
	data.Resize(data.GetLength() + 31 + 1);
	data.Resize(sprintf_s(data.GetData() + data.GetLength() - 1, data.GetCapacity() - data.GetLength() - 1, "%f", number) + 1);
}

void GTSL::String::Append(const double number)
{
	data.PopBack();
	data.PushBack(' ');
	data.Resize(data.GetLength() + 61 + 1);
	data.Resize(sprintf_s(data.GetData() + data.GetLength() - 1, data.GetCapacity() - data.GetLength() - 1, "%lf", number) + 1);
}

void GTSL::String::Insert(const char* cstring, const length_type index)
{
	data.Insert(index, const_cast<char*>(cstring), StringLength(cstring));
	return;
}

GTSL::String::length_type GTSL::String::FindFirst(const char c) const
{
	length_type i = 0;
	for (const auto& e : data) { if (e == c) { return i; } ++i; }
	return npos();
}

GTSL::String::length_type GTSL::String::FindLast(const char c) const
{
	length_type i = 0;
	for (auto& e : Ranger(data.end(), data.begin())) { if (e == c) { return i; } ++i; }
	return npos();
}

void GTSL::String::Drop(const length_type from)
{
	data.Resize(from + 1);
	data[from + 1] = '\0';
}

void GTSL::String::ReplaceAll(const char a, const char with)
{
	for (auto& c : data) { if (c == a) { c = with; } }
}

void GTSL::String::ReplaceAll(const char* a, const char* with)
{
	Array<uint32, 24, uint8> ocurrences; //cache ocurrences so as to not perform an array Resize every time we Find a match

	auto a_length = StringLength(a) - 1;
	auto with_length = StringLength(with) - 1;

	uint32 i = 0;
	
	while (true) //we don't know how long we will have to check for matches so keep looping until last if exits
	{
		ocurrences.Resize(0); //every time we enter loop set occurrences to 0

		while(ocurrences.GetLength() < ocurrences.GetCapacity() && i < data.GetLength() - 1) //while we don't exceed the occurrences array capacity and we are not At the end of the array(because we might hit the end in the first caching iteration)
		{
			if (data [i] == a[0]) //if current char matches the a's first character enter whole word loop check
			{
				uint32 j = 1;
				
				for (; j < a_length; ++j) //if the a text is matched add occurrence else quickly escape loop and go to next whole string loop
				{
					if (data[i + j] != a[j]) 
					{
						break;
					}
				}

				if (j == a_length - 1) //if loop found word MakeSpace occurrence and jump i by a's length
				{
					ocurrences.EmplaceBack(i + 1 - a_length);
					i += a_length;
				}
			}
			else //current char is not a match just check next in the following iteration
			{
				++i;
			}
		}

		const auto resize_size = ocurrences.GetLength() * (with_length - a_length);

		if (resize_size > 0)
		{
			data.Resize(data.GetLength() + resize_size);
		}

		for (auto& e : ocurrences)
		{
			data.MakeSpace(e, with_length - a_length);
			data.Place(with_length, const_cast<string_type*>(with), e);
		}

		if (i == data.GetLength() - 1) //if current index is last index in whole string break out of the loop
		{
			break;
		}
	}
}

constexpr GTSL::String::length_type GTSL::String::StringLength(const char* cstring)
{
	length_type length = 0;	while (*cstring) { ++length; }

	//We return Length + 1 to take into account for the null terminator character.
	return length + 1;
}

#define FSTRING_MAKESTRING_DEFAULT_SIZE 256

GTSL::String GTSL::String::MakeString(const char* cstring, ...)
{
	String Return(FSTRING_MAKESTRING_DEFAULT_SIZE);

	va_list vaargs;
	va_start(vaargs, cstring);
	const auto Count = snprintf(Return.data.GetData(), Return.data.GetLength(), cstring, vaargs) + 1;
	//Take into account null terminator.
	if (Count > Return.data.GetLength())
	{
		Return.data.Resize(Count);

		snprintf(Return.data.GetData(), Return.data.GetLength(), cstring, vaargs);
	}
	va_end(vaargs);

	return Return;
}

char GTSL::String::toLowerCase(char c)
{
	if ('A' <= c && c <= 'Z') return c += ('a' - 'A');
	return c;
}

char GTSL::String::toUpperCase(char c)
{
	if ('a' <= c && c <= 'z') return c += ('a' - 'A');
	return c;
}

OutStream& GTSL::String::operator<<(OutStream& archive)
{
	archive << data; return archive;
}

InStream& GTSL::String::operator>>(InStream& archive)
{
	archive >> data; return archive;
}