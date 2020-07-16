#include "GTSL/String.hpp"

#include "GTSL/Array.hpp"
#include "GTSL/Serialize.h"

using namespace GTSL;

String& String::operator=(const char* cstring)
{
	return *this;
}

bool String::operator==(const String& other) const
{
	//Discard if Length of strings is not equal, first because it helps us discard before even starting, second because we can't compare strings of different sizes.
	if (data.GetLength() != other.data.GetLength()) return false;

	length_type i = 0;
	for (const auto& c : data) { if (c != other.data[i]) { return false; } ++i; }
	return true;
}

bool String::NonSensitiveComp(const String& other) const
{
	//Discard if Length of strings is not equal, first because it helps us discard before even starting, second because we can't compare strings of different sizes.
	if (data.GetLength() != other.data.GetLength()) return false;

	length_type i = 0;
	for (const auto& c : data) { if (c != (toLowerCase(other.data[i]) || toUpperCase(other.data[i]))) { return false; } ++i; }

	return true;
}

void String::Append(const char* cstring, const AllocatorReference& allocatorReference)
{
	data.Place(data.GetLength(), ' '); //Push space.
	data.PushBack(Ranger<const UTF8>(StringLength(cstring), cstring), allocatorReference);
	data.PushBack(Ranger<const UTF8>(StringLength(cstring), cstring), allocatorReference);
	return;
}

void String::Append(const String& string, const AllocatorReference& allocatorReference)
{
	data.Place(data.GetLength(), ' '); //Push space.
	data.PushBack(string.data, allocatorReference); //Push new string.
	return;
}

void String::Append(const Ranger<const UTF8>& ranger, const AllocatorReference& allocatorReference)
{
	data.PushBack(ranger, allocatorReference);
}

void String::Append(const uint8 number, const AllocatorReference& allocatorReference)
{
	Ranger<string_type> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
	ToString(number, range);
	data.PushBack(range, allocatorReference);
}

void String::Append(const int8 number, const AllocatorReference& allocatorReference)
{
	Ranger<string_type> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
	ToString(number, range);
	data.PushBack(range, allocatorReference);
}

void String::Append(const uint16 number, const AllocatorReference& allocatorReference)
{
	Ranger<string_type> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
	ToString(number, range);
	data.PushBack(range, allocatorReference);
}

void String::Append(const int16 number, const AllocatorReference& allocatorReference)
{
	Ranger<string_type> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
	ToString(number, range);
	data.PushBack(range, allocatorReference);
}

void String::Append(const uint32 number, const AllocatorReference& allocatorReference)
{
	Ranger<string_type> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
	ToString(number, range);
	data.PushBack(range, allocatorReference);
}

void String::Append(const int32 number, const AllocatorReference& allocatorReference)
{
	Ranger<string_type> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
	ToString(number, range);
	data.PushBack(range, allocatorReference);
}

void String::Append(const uint64 number, const AllocatorReference& allocatorReference)
{
	Ranger<string_type> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
	ToString(number, range);
	data.PushBack(range, allocatorReference);
}

void String::Append(const int64 number, const AllocatorReference& allocatorReference)
{
	Ranger<string_type> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
	ToString(number, range);
	data.PushBack(range, allocatorReference);
}

void String::Append(const float32 number, const AllocatorReference& allocatorReference)
{
	Ranger<string_type> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
	ToString(number, range);
	data.PushBack(range, allocatorReference);
}

void String::Append(const float64 number, const AllocatorReference& allocatorReference)
{
	Ranger<string_type> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
	ToString(number, range);
	data.PushBack(range, allocatorReference);
}

String::length_type String::FindFirst(const char c) const
{
	length_type i = 0;
	for (const auto& e : data) { if (e == c) { return i; } ++i; }
	return npos();
}

String::length_type String::FindLast(const char c) const
{
	length_type i = 0;
	for (const auto& e : data) { if (e == c) { return i; } ++i; }
	return npos();
}

void String::Drop(const length_type from)
{
	data.ResizeDown(from);
}

void String::ReplaceAll(const char a, const char with)
{
	for (auto& c : data) { if (c == a) { c = with; } }
}

void String::ReplaceAll(const char* a, const char* with, const AllocatorReference& allocatorReference)
{
	Array<uint32, 24> ocurrences; //cache ocurrences so as to not perform an array Resize every time we Find a match

	const auto a_length = StringLength(a) - 1;
	const auto with_length = StringLength(with) - 1;

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
			data.Resize(data.GetLength() + resize_size, allocatorReference);
		}

		for (auto& e : ocurrences)
		{
			data.Insert(e, GTSL::Ranger<const UTF8>(with_length, with), allocatorReference);
		}

		if (i == data.GetLength() - 1) //if current index is last index in whole string break out of the loop
		{
			break;
		}
	}
}

char String::toLowerCase(char c)
{
	if ('A' <= c && c <= 'Z') return c += ('a' - 'A');
	return c;
}

char String::toUpperCase(char c)
{
	if ('a' <= c && c <= 'z') return c += ('a' - 'A');
	return c;
}

//OutStream& GTSL::operator<<(OutStream& os, const String& string)
//{
//	os << string.data; return os;
//}
//
//InStream& GTSL::operator>>(InStream& inStream, String& string)
//{
//	inStream >> string.data; return inStream;
//}
