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
		using string_type = UTF8;
		using length_type = typename Vector<string_type, ALLOCATOR>::length_type;
		
		String() = default;

		String(const char* cstring, const ALLOCATOR& allocatorReference) : data(Range<const UTF8*>(StringLength(cstring) + 1, cstring), allocatorReference) {}

		/**
		 * \brief Creates an String with enough space allocated for length elements.
		 * \param length Amount of elements to allocate.
		 */
		explicit String(const length_type length, const ALLOCATOR& allocatorReference) : data(length, allocatorReference) {}

		String(const String& other) = default;
		String(String&& other) noexcept = default;
		String& operator=(const String& other) = default;
		String& operator=(String&& other) noexcept = default;
		~String() = default;

		String& operator=(const Range<const UTF8*> range)
		{
			data.ResizeDown(0);
			data.PushBack(range);
			return *this;
		}

		string_type operator[](const length_type i) const noexcept { return data[i]; }

		auto begin() noexcept { return data.begin(); }
		[[nodiscard]] auto begin() const noexcept { return data.begin(); }
		auto end() noexcept { return data.end(); }
		[[nodiscard]] auto end() const noexcept { return data.end(); }

		[[nodiscard]] length_type npos() const { return data.GetLength() + 1; }

		//Returns true if the two String's contents are the same. Comparison is case sensitive.
		bool operator==(const String& other) const
		{
			//Discard if Length of strings is not equal, first because it helps us discard before even starting, second because we can't compare strings of different sizes.
			if (data.GetLength() != other.data.GetLength()) return false;

			length_type i = 0;
			for (const auto& c : data) { if (c != other.data[i]) { return false; } ++i; }
			return true;
		}

		//Returns true if the two String's contents are the same. Comparison is case insensitive.
		[[nodiscard]] bool NonSensitiveComp(const String& other) const
		{
			//Discard if Length of strings is not equal, first because it helps us discard before even starting, second because we can't compare strings of different sizes.
			if (data.GetLength() != other.data.GetLength()) return false;

			length_type i = 0;
			for (const auto& c : data) { if (c != (ToLowerCase(other.data[i]) || ToUpperCase(other.data[i]))) { return false; } ++i; }

			return true;
		}

		//Returns the contents of this String as a C-String.
		char* c_str() { return data.GetData(); }

		operator Range<UTF8*>() const { return Range<UTF8*>(data); }
		operator Range<const UTF8*>() const { return Range<const UTF8*>(data); }

		[[nodiscard]] const char* c_str() const { return data.GetData(); }

		//Return the length of this String. Does not take into account the null terminator character.
		[[nodiscard]] length_type GetLength() const { return data.GetLength() - 1; }
		//Returns whether this String is empty.
		[[nodiscard]] bool IsEmpty() const { return data.GetLength() == 0; }

		//Places a the c-string after this String with a space in the middle.
		void Append(const char* cstring)
		{
			data.Place(data.GetLength(), ' '); //Push space.
			data.PushBack(Range<const UTF8*>(StringLength(cstring), cstring));
			data.PushBack(Range<const UTF8*>(StringLength(cstring), cstring));
		}
		
		//Places the String after this String with a space in the middle.
		void Append(const String& string)
		{
			data.Place(data.GetLength(), ' '); //Push space.
			data.PushBack(string.data); //Push new string.
		}

		void Append(const Range<const UTF8*> ranger)
		{
			data.PushBack(ranger);
		}

		void Append(const uint8 number)
		{
			Range<string_type*> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
			ToString(number, range);
			data.PushBack(range);
		}
		void Append(const int8 number)
		{
			Range<string_type*> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
			ToString(number, range);
			data.PushBack(range);
		}
		void Append(const uint16 number)
		{
			Range<string_type*> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
			ToString(number, range);
			data.PushBack(range);
		}
		void Append(const int16 number)
		{
			Range<string_type*> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
			ToString(number, range);
			data.PushBack(range);
		}
		void Append(const uint32 number)
		{
			Range<string_type*> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
			ToString(number, range);
			data.PushBack(range);
		}
		void Append(const int32 number)
		{
			Range<string_type*> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
			ToString(number, range);
			data.PushBack(range);
		}
		void Append(const uint64 number)
		{
			Range<string_type*> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
			ToString(number, range);
			data.PushBack(range);
		}
		void Append(const int64 number)
		{
			Range<string_type*> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
			ToString(number, range);
			data.PushBack(range);
		}
		void Append(const float32 number)
		{
			Range<string_type*> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
			ToString(number, range);
			data.PushBack(range);
		}
		void Append(const float64 number)
		{
			Range<string_type*> range(data.begin() + data.GetLength(), data.begin() + data.GetCapacity());
			ToString(number, range);
			data.PushBack(range);
		}

		/**
		* \brief Returns an index to the first char in the string that is equal to c. If no such character is found npos() is returned.
		* \param c Char to Find.
		* \return Index to found char.
		*/
		[[nodiscard]] length_type FindFirst(char c) const
		{
			length_type i = 0;
			for (const auto& e : data) { if (e == c) { return i; } ++i; }
			return npos();
		}

		/**
		 * \brief Returns an index to the last char in the string that is equal to c. If no such character is found npos() is returned.
		 * \param c Char to Find.
		 * \return Index to found char.
		 */
		[[nodiscard]] length_type FindLast(char c) const
		{
			length_type i = 0;
			for (const auto& e : data) { if (e == c) { return i; } ++i; }
			return npos();
		}

		/**
		 * \brief Drops/removes the parts of the string from from forward.
		 * \param from index to cut forward from.
		 */
		void Drop(length_type from)
		{
			data.ResizeDown(from);
		}

		void ReplaceAll(char a, char with)
		{
			for (auto& c : data) { if (c == a) { c = with; } }
		}
		
		void ReplaceAll(const char* a, const char* with)
		{
			Array<uint32, 24> ocurrences; //cache ocurrences so as to not perform an array Resize every time we Find a match

			const auto a_length = StringLength(a) - 1;
			const auto with_length = StringLength(with) - 1;

			uint32 i = 0;

			while (true) //we don't know how long we will have to check for matches so keep looping until last if exits
			{
				ocurrences.Resize(0); //every time we enter loop set occurrences to 0

				while (ocurrences.GetLength() < ocurrences.GetCapacity() && i < data.GetLength() - 1) //while we don't exceed the occurrences array capacity and we are not At the end of the array(because we might hit the end in the first caching iteration)
				{
					if (data[i] == a[0]) //if current char matches the a's first character enter whole word loop check
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
					data.Insert(e, Range<const UTF8*>(with_length, with));
				}

				if (i == data.GetLength() - 1) //if current index is last index in whole string break out of the loop
				{
					break;
				}
			}
		}
		
	private:
		Vector<string_type, ALLOCATOR> data;
	};
}
