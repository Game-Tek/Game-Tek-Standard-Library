#pragma once

#include "Core.h"

#include "Vector.hpp"
#include "Stream.h"
#include "StringCommon.h"

namespace GTSL
{
	class String
	{
		using string_type = UTF8;
		using length_type = Vector<string_type>::length_type;
	public:
		//Constructs an Empty String.
		String() = default;

		//template<size_t N>
		//String(const char(&_Literal)[N]) : data(N, *_Literal)
		//{
		//}

		String(const char* cstring, const AllocatorReference& allocatorReference) : data(GTSL::Ranger<const UTF8>(StringLength(cstring) + 1, cstring), allocatorReference) {}

		/**
		 * \brief Creates an String with enough space allocated for length elements.
		 * \param length Amount of elements to allocate.
		 */
		explicit String(const length_type length, const AllocatorReference& allocatorReference) : data(length, allocatorReference) {}

		/**
		 * \brief Creates an String from a length and an String. Assumes the string has no null terminator character. If the string you pass happens to have a null terminator you should insert one character less.
		 * \param length Length to use from the String.
		 * \param string String to copy characters from.
		 */
		String(length_type length, const String& string, const AllocatorReference& allocatorReference);

		/**
		 * \brief Creates an String from a length an String and an offset. Assumes the string has no null terminator character. If the string you pass happens to have a null terminator you should insert one character less.
		 * \param length Length to use from the String.
		 * \param string String to copy from.
		 * \param offset Offset from the start of the string to start copying from.
		 */
		String(length_type length, const String& string, length_type offset, const AllocatorReference& allocatorReference);

		String(const String& other) = default;
		String(String&& other) noexcept = default;
		String& operator=(const String& other) = default;
		String& operator=(String&& other) noexcept = default;
		void Free(const AllocatorReference& allocatorReference) { data.Free(allocatorReference); }
		~String() = default;
		
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

		operator Ranger<UTF8>() { return Ranger<UTF8>(data); }
		operator Ranger<const UTF8>() const { return Ranger<const UTF8>(data); }

		[[nodiscard]] const char* c_str() const { return data.GetData(); }

		//Return the length of this String. Does not take into account the null terminator character.
		[[nodiscard]] length_type GetLength() const { return data.GetLength() - 1; }
		//Returns whether this String is empty.
		[[nodiscard]] bool IsEmpty() const { return data.GetLength() == 0; }

		//Places a the c-string after this String with a space in the middle.
		void Append(const char* cstring, const AllocatorReference& allocatorReference);
		//Places the String after this String with a space in the middle.
		void Append(const String& string, const AllocatorReference& allocatorReference);

		void Append(const Ranger<const UTF8>& ranger, const AllocatorReference& allocatorReference);

		void Append(uint8 number, const AllocatorReference& allocatorReference);
		void Append(int8 number, const AllocatorReference& allocatorReference);
		void Append(uint16 number, const AllocatorReference& allocatorReference);
		void Append(int16 number, const AllocatorReference& allocatorReference);
		void Append(uint32 number, const AllocatorReference& allocatorReference);
		void Append(int32 number, const AllocatorReference& allocatorReference);
		void Append(uint64 number, const AllocatorReference& allocatorReference);
		void Append(int64 number, const AllocatorReference& allocatorReference);
		void Append(float32 number, const AllocatorReference& allocatorReference);
		void Append(float64 number, const AllocatorReference& allocatorReference);

		/**
		 * \brief Places cstring At the specified index.
		 * \param cstring C-String to insert in the string.
		 * \param index Index At which to Place the cstring.
		 */
		void Insert(const char* cstring, length_type index);

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
		void ReplaceAll(const char* a, const char* with, const AllocatorReference& allocatorReference);
		
	private:
		Vector<string_type> data;

		friend class InStream;
		friend class OutStream;

		static char toLowerCase(char c);
		static char toUpperCase(char c);
		
		friend OutStream& operator<<(OutStream& os, const String& string);
		friend InStream& operator>>(InStream& inStream, String& string);
	};
}
