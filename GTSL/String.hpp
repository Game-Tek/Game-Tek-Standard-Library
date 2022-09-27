#pragma once

#include "Core.h"
#include "StringCommon.h"
#include "Allocator.h"
#include "Unicode.hpp"

namespace GTSL
{
	//UTF-8
	template<class ALLOCATOR>
	class String {
	public:
		using string_type = char8_t;

		String(const ALLOCATOR& allocatorReference = ALLOCATOR()) : allocator(allocatorReference) {
		}

		/**
		 * \brief Creates an String with enough space allocated for length elements.
		 * \param initialCapacity Amount of elements to allocate.
		 */
		String(const uint32 initialCapacity, const ALLOCATOR& allocatorReference = ALLOCATOR()) : String(allocatorReference) {
			tryResize(initialCapacity);
		}

		String(const Range<const char8_t*> range, const ALLOCATOR& allocatorReference = ALLOCATOR()) : String(allocatorReference) {
			copy(range);
		}

		String(const String& other) : allocator(other.allocator) {
			copy(other);
		}

		String(String&& other) noexcept requires std::move_constructible<ALLOCATOR> : allocator(MoveRef(other.allocator)), data(other.data), codePoints(other.codePoints), bytes(other.bytes), capacity(other.capacity) {
			other.data = nullptr; other.bytes = 0; other.codePoints = 0; other.capacity = 0;
		}

		String& operator=(const StringView range) {
			bytes = 0;
			codePoints = 0;
			copy(range);
			return *this;
		}

		String& operator=(const String& other) {
			bytes = 0; //reset length
			codePoints = 0;
			copy(other);
			return *this;
		}

		String& operator=(String&& other) noexcept requires std::move_constructible<ALLOCATOR> {
			//free current
			allocator = MoveRef(other.allocator);
			data = other.data;
			bytes = other.bytes;
			codePoints = other.codePoints;
			capacity = other.capacity;

			other.bytes = 0; other.codePoints = 0; other.capacity = 0; other.data = nullptr;

			return *this;
		}

		~String() {
			if (data) { allocator.Deallocate(capacity, 16, data); }
		}

		char32_t operator[](const uint32 cp) const noexcept {
			auto pos = getByteAndLengthForCodePoint(cp);
			auto d = data + Get<0>(pos);
			return ToUTF32(d[0], d[1], d[2], d[3], Get<2>(pos)).Get();
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

		//auto begin() noexcept { return Iterator(data, bytes); }
		[[nodiscard]] auto begin() const noexcept { return StringIterator(data, bytes, 0); }
		//auto end() noexcept { return data + bytes + 1; }
		[[nodiscard]] auto end() const noexcept { return StringIterator(data, bytes, bytes); }

		//operator Range<char8_t*>() const { return Range<char8_t*>(begin(), end()); }
		operator Range<const char8_t*>() const { return Range<const char8_t*>(GetBytes(), GetCodepoints(), data); }

		[[nodiscard]] const char8_t* c_str() const { return data; }

		//Return the length of this String.
		[[nodiscard]] uint32 GetBytes() const { return bytes; }
		[[nodiscard]] uint32 GetCodepoints() const { return codePoints; }
		[[nodiscard]] uint32 GetCapacity() const { return capacity; }
		//Returns whether this String is empty.
		[[nodiscard]] bool IsEmpty() const { return !bytes; }

		friend bool operator==(const String& string, const GTSL::StringView string_view) {
			return GTSL::StringView(string) == string_view;
		}

		explicit operator bool() const { return bytes; }
		friend bool operator<(const uint32 i, const String& string) {
			return i < string.GetCodepoints();
		}		

		String operator+(Range<const char8_t*> range) const {
			String res(*this, allocator);
			res += range;
			return res;
		}

		String operator&(Range<const char8_t*> range) const {
			String res(*this, allocator);
			res += u8' '; res += range;
			return res;
		}

		String operator&(uint32 number) const {
			String res(*this, allocator);
			res += u8' ';
			ToString(res, number);
			return res;
		}

		String& operator+=(Range<const char8_t*> range) {
			copy(range);
			return *this;
		}

		String& operator&=(Range<const char8_t*> range) {
			(*this) += u8' ';
			copy(range);
			return *this;
		}

		String& operator+=(char8_t character) {
			tryResize(1 + 3/*res len*/);
			data[bytes++] = character;
			for (uint8 i = 0; i < 4; ++i)
				data[bytes + i] = u8'\0';
			++codePoints;
			return *this;
		}

		String& operator<<(StringView other) {
			(*this) += other;
			return *this;
		}

		String& operator+=(const Join& join) {			
			if(join.Strings.ElementCount()) {
				(*this) += join.Strings[0];
			} else {
				return (*this);
			}

			for(auto begin = join.Strings.begin() + 1; begin != join.Strings.end(); ++begin) {
				(*this) += join.Connector;
				(*this) += *begin;
			}

			return *this;
		}

		//String& operator+=(char32_t character) {
		//	auto p = ToUTF8(character);
		//	bytes += utf8_length(p[0]);
		//	++codePoints;
		//	return *this;
		//}

		/**
		 * \brief Drops/removes the parts of the string from from forward.
		 * \param from index to cut forward from.
		 */
		void Drop(uint32 cp) {
			auto pos = getByteAndLengthForCodePoint(cp);
			bytes = Get<0>(pos); data[Get<0>(pos)] = u8'\0';
			codePoints = Get<1>(pos);
			for (uint8 i = 0; i < 4; ++i)
				data[Get<0>(pos) + i] = u8'\0';
		}

		void Resize(const uint32 newLength) {
			tryResize(newLength);
		}

		//void ReplaceAll(const char* a, const char* with) {
		//	Array<uint32, 24> ocurrences; //cache ocurrences so as to not perform an array Resize every time we Find a match
		//
		//	const auto a_length = StringLength(a) - 1;
		//	const auto with_length = StringLength(with) - 1;
		//
		//	uint32 i = 0;
		//
		//	while (true) { //we don't know how long we will have to check for matches so keep looping until last if exits
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

		friend void ReplaceAll(String& string, char8_t a, char8_t with) {
			for (uint32 i = 0; i < string.bytes; ++i) { if (string.data[i] == a) { string.data[i] = with; } }
		}

		/**
		 * \brief Returns an index to the last char in the string that is equal to c. If no such character is found npos() is returned.
		 * \param c Char to Find.
		 * \return Index to found char.
		 */
		[[nodiscard]] friend Result<uint32> FindLast(const String& string, char8_t c)
		{
			for (uint32 i = string.GetBytes() - 1; i < string.GetBytes(); --i) { if (string.data[i] == c) { return Result(MoveRef(i), true); } }
			return Result<uint32>(false);
		}

		/**
		 * \brief Chops an string at the first occurrence of a character in the string and shifts the chop position by the nudge amount.
		 * \param string String to modify.
		 * \param c Character to search for.
		 * \param nudge Amount to shift codepoint to be dropped.
		 * \return Whether or not a drop has occurred.
		 */
		friend bool RTrimFirst(String& string, char8_t c, int32 nudge = 0) {
			auto pos = FindFirst(string, c);
			if (pos.State()) {
				string.Drop(pos.Get() + nudge);
				return true;
			}
			return false;
		}

		/**
		 * \brief Chops an string at the last occurrence of a character in the string and shifts the chop position by the nudge amount.
		 * \param string String to modify.
		 * \param c Character to search for.
		 * \param nudge Amount to shift codepoint to be dropped.
		 * \return Whether or not a drop has occurred.
		 */
		friend bool RTrimLast(String& string, char8_t c, int32 nudge = 0) {
			auto pos = FindLast(string, c);
			if (pos.State()) {
				string.Drop(pos.Get() + nudge);
				return true;
			}
			return false;
		}

		/**
		 * \brief Chops an string at the first occurrence of a character in the string and shifts the chop position by the nudge amount.
		 * \param string String to modify.
		 * \param c Character to search for.
		 * \param nudge Amount to shift codepoint to be dropped.
		 * \return Whether or not a drop has occurred.
		 */
		friend bool LTrimFirst(String& string, char8_t c, int32 nudge = 0) {
			auto pos = FindFirst(string, c);
			if (pos.State()) {
				auto p = string.getByteAndLengthForCodePoint(pos.Get() + 1 + nudge);

				string.bytes -= Get<0>(p);

				for(uint32 i = 0, j = Get<0>(p); i < string.bytes; ++i, ++j) {
					string.data[i] = string.data[j];
				}

				string.codePoints -= Get<1>(p);

				for (uint8 i = 0; i < 4; ++i)
					string.data[string.bytes + i] = u8'\0';

				return true;
			}
			return false;
		}

		friend bool LTrimLast(String& string, char8_t c, int32 nudge = 0) {
			auto pos = FindLast(string, c);
			if (pos.State()) {
				auto p = string.getByteAndLengthForCodePoint(pos.Get() + 1 + nudge);

				string.bytes -= Get<0>(p);

				for(uint32 i = 0, j = Get<0>(p); i < string.bytes; ++i, ++j) {
					string.data[i] = string.data[j];
				}

				string.codePoints -= Get<1>(p);

				for (uint8 i = 0; i < 4; ++i)
					string.data[string.bytes + i] = u8'\0';

				return true;
			}
			return false;
		}

		/**
		* \brief Returns an index to the first char in the string that is equal to c. If no such character is found npos() is returned.
		* \param c Char to Find.
		* \return Index to found char.
		*/
		friend Result<uint32> FindFirst(const String& string, char32_t c) {
			for (uint32 i = 0; auto e : string) { if (e == c) { return Result(MoveRef(i), true); } ++i; }
			return Result<uint32>(false);
		}
	
	private:
		[[no_unique_address]] ALLOCATOR allocator;
		char8_t* data = nullptr;
		uint32 codePoints = 0, bytes = 0, capacity = 0;

		uint32 getCodepoints() const { return codePoints; }
		uint32 getBytes() const { return bytes; }

		//bytes to codepoint, codepoints to codepoint, length of codepoint
		Tuple<uint32, uint32, uint8> getByteAndLengthForCodePoint(uint32 cp) const {
			uint32 codePoint = 0; uint32 byt = 0; uint8 len = 0;

			while (codePoint != cp) {
				++codePoint;
				len = UTF8CodePointLength(data[byt]);
				byt += len;
			}

			len = UTF8CodePointLength(data[byt]);

			return Tuple(MoveRef(byt), MoveRef(codePoint), MoveRef(len));
		}

		void tryResize(uint32 newSize) {
			newSize += 3; /*null terminator padding*/

			if (newSize > capacity) {				
				if(data) {
					GTSL::Resize(allocator, &data, &capacity, newSize * 2, bytes);
				} else {
					Allocate(allocator, newSize, &data, &capacity);
				}
			}
		}
		
		void copy(const Range<const char8_t*> string) {
			tryResize(bytes + string.GetBytes());
			for(uint32 i = 0; i < string.GetBytes(); ++i) { data[bytes + i] = string.GetData()[i]; }
			bytes += string.GetBytes(); codePoints += string.GetCodepoints();
			for (uint32 i = 0, pos = bytes; i < 3; ++i, ++pos) { data[pos] = u8'\0'; }
		}

		//friend class String;

	public:
		friend void Insert(const String& string, auto& buffer) {
			Insert(string.bytes, buffer); Insert(string.codePoints, buffer);
			buffer.Write(string.GetBytes() + 1, reinterpret_cast<const byte*>(string.c_str()));
		}
		
		friend void Extract(String& string, auto& buffer) {
			uint32 bytes = 0, codepoints = 0;
			Extract(bytes, buffer); Extract(codepoints, buffer);
			::new(&string) String(bytes);
			buffer.Read(bytes + 1, reinterpret_cast<byte*>(string.data));
			string.bytes = bytes;
			string.codePoints = codepoints;
		}
	};

	template<uint64 N>
	using StaticString = String<StaticAllocator<N>>;

	template<class ALLOC, uint64 N>
	using SemiString = String<DoubleAllocator<N, ALLOC>>;
}
