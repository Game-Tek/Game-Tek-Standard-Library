#pragma once

#include "Array.hpp"
#include "Core.h"

#include "Vector.hpp"
#include "StringCommon.h"

namespace GTSL
{
	template<class ALLOCATOR>
	class String
	{
	public:
		using string_type = char8_t;
		using length_type = typename Vector<string_type, ALLOCATOR>::length_type;
		
		String() = default;

		/**
		 * \brief Creates an String with enough space allocated for length elements.
		 * \param length Amount of elements to allocate.
		 */
		explicit String(const length_type length, const ALLOCATOR& allocatorReference) : allocator(allocatorReference)
		{
			uint64 allocatedSize = 0;
			allocator.Allocate(length, 16, reinterpret_cast<void**>(&data), &allocatedSize);
			capacity = static_cast<uint32>(allocatedSize / sizeof(string_type));
		}
		
		String(const char8_t* cstring, const ALLOCATOR& allocatorReference) : String(StringLength(cstring), allocatorReference)
		{
			auto stringLength = StringLength(cstring);
			copy(stringLength, cstring);
			length = stringLength - 1;
		}

		String(const String& other) = default;
		String(String&& other) noexcept = default;
		String& operator=(const String& other) = default;
		String& operator=(String&& other) noexcept = default;
		
		~String()
		{
			if (data) { allocator.Deallocate(capacity, 16, data); }
		};

		string_type operator[](const length_type i) const noexcept { return data[i]; }

		auto begin() noexcept { return data; }
		[[nodiscard]] auto begin() const noexcept { return data; }
		auto end() noexcept { return data + length + 1; }
		[[nodiscard]] auto end() const noexcept { return data + length + 1; }

		//Returns true if the two String's contents are the same. Comparison is case sensitive.
		bool operator==(const String& other) const
		{
			//Discard if Length of strings is not equal, first because it helps us discard before even starting, second because we can't compare strings of different sizes.
			if (length != other.length) return false;

			length_type i = 0;
			for (const auto& c : data) { if (c != other.data[i]) { return false; } ++i; }
			return true;
		}

		////Returns true if the two String's contents are the same. Comparison is case insensitive.
		//[[nodiscard]] bool NonSensitiveComp(const String& other) const
		//{
		//	//Discard if Length of strings is not equal, first because it helps us discard before even starting, second because we can't compare strings of different sizes.
		//	if (data.GetLength() != other.data.GetLength()) return false;
		//
		//	length_type i = 0;
		//	for (const auto& c : data) { if (c != (ToLowerCase(other.data[i]) || ToUpperCase(other.data[i]))) { return false; } ++i; }
		//
		//	return true;
		//}

		operator Range<char8_t*>() const { return Range<char8_t*>(begin(), end()); }
		operator Range<const char8_t*>() const { return Range<const char8_t*>(begin(), end()); }

		[[nodiscard]] const char8_t* c_str() const { return data; }

		//Return the length of this String. Does not take into account the null terminator character.
		[[nodiscard]] length_type GetLength() const { return length + 1; }
		//Returns whether this String is empty.
		[[nodiscard]] bool IsEmpty() const { return !length; }

		String& operator+=(const char8_t* string)
		{
			auto stringLength = StringLength(string);
			copy(stringLength, string);
			length += stringLength - 1;
			return *this;
		}

		String& operator+=(Range<const char8_t*> range)
		{
			auto stringLength = range.ElementCount();
			copy(stringLength, range.begin());
			length += stringLength - 1;
			return *this;
		}

		String& operator+=(char8_t character)
		{
			data[length] = character;
			++length;
			return *this;
		}
		
		/**
		* \brief Returns an index to the first char in the string that is equal to c. If no such character is found npos() is returned.
		* \param c Char to Find.
		* \return Index to found char.
		*/
		[[nodiscard]] Result<length_type> FindFirst(char8_t c) const
		{
			length_type i = 0;
			for (const auto& e : data) { if (e == c) { return Result<length_type>(MoveRef(i), true); } ++i; }
			return Result<length_type>(false);
		}

		/**
		 * \brief Returns an index to the last char in the string that is equal to c. If no such character is found npos() is returned.
		 * \param c Char to Find.
		 * \return Index to found char.
		 */
		[[nodiscard]] Result<Pair<length_type, uint32>> FindLast(char8_t c) const
		{
			length_type i = 1, t = GetLength();
			for (const auto& e : data) { if (e == c) { return Result<Pair<length_type, uint32>>(Pair<length_type, uint32>(i, t), true); } ++i; --t; }
			return Result<Pair<length_type, uint32>>(false);
		}

		/**
		 * \brief Drops/removes the parts of the string from from forward.
		 * \param from index to cut forward from.
		 */
		void Drop(length_type from)
		{
		}

		void ReplaceAll(char8_t a, char8_t with)
		{
			for (auto& c : data) { if (c == a) { c = with; } }
		}
		
		//void ReplaceAll(const char* a, const char* with)
		//{
		//	Array<uint32, 24> ocurrences; //cache ocurrences so as to not perform an array Resize every time we Find a match
		//
		//	const auto a_length = StringLength(a) - 1;
		//	const auto with_length = StringLength(with) - 1;
		//
		//	uint32 i = 0;
		//
		//	while (true) //we don't know how long we will have to check for matches so keep looping until last if exits
		//	{
		//		ocurrences.Resize(0); //every time we enter loop set occurrences to 0
		//
		//		while (ocurrences.GetLength() < ocurrences.GetCapacity() && i < data.GetLength() - 1) //while we don't exceed the occurrences array capacity and we are not At the end of the array(because we might hit the end in the first caching iteration)
		//		{
		//			if (data[i] == a[0]) //if current char matches the a's first character enter whole word loop check
		//			{
		//				uint32 j = 1;
		//
		//				for (; j < a_length; ++j) //if the a text is matched add occurrence else quickly escape loop and go to next whole string loop
		//				{
		//					if (data[i + j] != a[j])
		//					{
		//						break;
		//					}
		//				}
		//
		//				if (j == a_length - 1) //if loop found word MakeSpace occurrence and jump i by a's length
		//				{
		//					ocurrences.EmplaceBack(i + 1 - a_length);
		//					i += a_length;
		//				}
		//			}
		//			else //current char is not a match just check next in the following iteration
		//			{
		//				++i;
		//			}
		//		}
		//
		//		const auto resize_size = ocurrences.GetLength() * (with_length - a_length);
		//
		//		if (resize_size > 0)
		//		{
		//			data.Resize(data.GetLength() + resize_size);
		//		}
		//
		//		for (auto& e : ocurrences)
		//		{
		//			data.Insert(e, Range<const char8_t*>(with_length, with));
		//		}
		//
		//		if (i == data.GetLength() - 1) //if current index is last index in whole string break out of the loop
		//		{
		//			break;
		//		}
		//	}
		//}
		
	private:
		char8_t* data = nullptr; uint32 length = 0, capacity = 0;
		[[no_unique_address]] ALLOCATOR allocator;

		void copy(uint32 stringLength, const string_type* string)
		{
			GTSL_ASSERT(stringLength + length < capacity, "Not enough space");
			for (uint32 i = 0, t = length; i < stringLength; ++i, ++t) { data[t] = string[i]; }
		}
	};
}
