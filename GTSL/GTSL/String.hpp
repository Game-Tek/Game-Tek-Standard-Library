#pragma once

#include "Core.h"

#include "Vector.hpp"
#include "Stream.h"

namespace GTSL
{
	class String
	{
		using string_type = char;
		using length_type = Vector<string_type>::length_type;
	public:
		//Constructs an Empty String.
		String() = default;

		//template<size_t N>
		//String(const char(&_Literal)[N]) : data(N, *_Literal)
		//{
		//}

		String(const char* cstring, AllocatorReference* allocatorReference) : data(StringLength(cstring), const_cast<char*>(cstring), allocatorReference) {}

		/**
		 * \brief Creates an String with enough space allocated for length elements.
		 * \param length Amount of elements to allocate.
		 */
		explicit String(const length_type length, AllocatorReference* allocatorReference) : data(length, allocatorReference) {}

		/**
		 * \brief Creates an String from a length and an array. Assumes the array has no null terminator character. If the array you pass happens to have a null terminator you should insert one character less.
		 * \param length length to use from the cstring array
		 * \param cstring array to copy from
		 */
		String(const length_type length, const char* cstring, AllocatorReference* allocatorReference);

		/**
		 * \brief Creates an String from a length and an String. Assumes the string has no null terminator character. If the string you pass happens to have a null terminator you should insert one character less.
		 * \param length Length to use from the String.
		 * \param string String to copy characters from.
		 */
		String(const length_type length, const String& string, AllocatorReference* allocatorReference);

		/**
		 * \brief Creates an String from a length an String and an offset. Assumes the string has no null terminator character. If the string you pass happens to have a null terminator you should insert one character less.
		 * \param length Length to use from the String.
		 * \param string String to copy from.
		 * \param offset Offset from the start of the string to start copying from.
		 */
		String(const length_type length, const String& string, const length_type offset, AllocatorReference* allocatorReference);

		String(const String& other) = default;
		String(String&& other) noexcept = default;
		String& operator=(const String& other) = default;
		String& operator=(String&& other) noexcept = default;
		~String() = default;

		[[nodiscard]] AllocatorReference* GetAllocatorReference() const { return data.GetAllocatorReference(); }
		
		String& operator=(const char* cstring);
		String& operator+=(char c);
		String& operator+=(const char* cstring);
		String& operator+=(const String& string);

		string_type operator[](const length_type i) noexcept { return data[i]; }
		string_type operator[](const length_type i) const noexcept { return data[i]; }

		auto begin() noexcept { return data.begin(); }
		[[nodiscard]] auto begin() const noexcept { return data.begin(); }
		auto end() noexcept { return data.end(); }
		[[nodiscard]] auto end() const noexcept { return data.end(); }

		[[nodiscard]] length_type npos() const { return data.GetLength() + 1; }

		//Returns true if the two String's contents are the same. Comparison is case sensitive.
		bool operator==(const String& other) const;

		//Returns true if the two String's contents are the same. Comparison is case insensitive.
		[[nodiscard]] bool NonSensitiveComp(const String& other) const;

		//Returns the contents of this String as a C-String.
		char* c_str() { return data.GetData(); }

		//Returns the contents of this String as a C-String.
		[[nodiscard]] const char* c_str() const { return data.GetData(); }

		//Return the length of this String. Does not take into account the null terminator character.
		[[nodiscard]] length_type GetLength() const { return data.GetLength() - 1; }
		//Returns whether this String is empty.
		[[nodiscard]] bool IsEmpty() const { return data.GetLength() == 0; }

		//Places a the c-string after this String with a space in the middle.
		void Append(const char* cstring);
		//Places the String after this String with a space in the middle.
		void Append(const String& string);

		void Append(uint8 number);
		void Append(int8 number);
		void Append(uint16 number);
		void Append(int16 number);
		void Append(uint32 number);
		void Append(int32 number);
		void Append(uint64 number);
		void Append(int64 number);
		void Append(float number);
		void Append(double number);

		/**
		 * \brief Places cstring At the specified index.
		 * \param cstring C-String to insert in the string.
		 * \param index Index At which to Place the cstring.
		 */
		void Insert(const char* cstring, const length_type index);

		/**
		* \brief Returns an index to the first char in the string that is equal to c. If no such character is found npos() is returned.
		* \param c Char to Find.
		* \return Index to found char.
		*/
		[[nodiscard]] length_type FindFirst(char c) const;

		/**
		 * \brief Returns an index to the last char in the string that is equal to c. If no such character is found npos() is returned.
		 * \param c Char to Find.
		 * \return Index to found char.
		 */
		[[nodiscard]] length_type FindLast(char c) const;

		/**
		 * \brief Drops/removes the parts of the string from from forward.
		 * \param from index to cut forward from.
		 */
		void Drop(length_type from);

		void ReplaceAll(char a, char with);
		void ReplaceAll(const char* a, const char* with);

		//Returns the length of the C-String accounting for the null terminator character. C-String MUST BE NULL TERMINATED.
		constexpr static length_type StringLength(const char* cstring);

		static String MakeString(const char* cstring, ...);
	private:
		Vector<string_type> data;

		friend class InStream;
		friend class OutStream;

		static char toLowerCase(char c);
		static char toUpperCase(char c);
		
		OutStream& operator<<(OutStream& archive);
		InStream& operator>>(InStream& archive);
	};
}
