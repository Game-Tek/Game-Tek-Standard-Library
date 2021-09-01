#pragma once

#include <concepts>

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
		String(const uint32 initialCapacity, const ALLOCATOR& allocatorReference = ALLOCATOR()) : String(allocatorReference)
		{
			tryResize(initialCapacity, 4);
		}
		
		String(const char8_t* cstring, const ALLOCATOR& allocatorReference = ALLOCATOR()) : String(allocatorReference) {
			copy(cstring);
		}

		String(const Range<const char8_t*> range, const ALLOCATOR& allocatorReference = ALLOCATOR()) : String(static_cast<uint32>(range.ElementCount()), allocatorReference) {
			copy(range);
		}

		String(const String& other) : allocator(other.allocator)
		{
			uint64 allocatedSize = 0;
			allocator.Allocate(other.length + 1, 16, reinterpret_cast<void**>(&data), &allocatedSize);
			capacity = static_cast<uint32>(allocatedSize / sizeof(string_type));
			copy(other.bytes, other.data);
			bytes = other.bytes;
			codePoints = other.codePoints;
			data[bytes] = u8'\0';
		}
		
		template<class B>
		String(const String<B>& other, const ALLOCATOR& allocatorReference = ALLOCATOR()) : allocator(allocatorReference)
		{
			uint64 allocatedSize = 0;
			allocator.Allocate(other.bytes + 1, 16, reinterpret_cast<void**>(&data), &allocatedSize);
			capacity = static_cast<uint32>(allocatedSize / sizeof(string_type));
			copy(other.bytes, other.data);
			bytes = other.bytes;
			codePoints = other.codePoints;
			data[bytes] = u8'\0';
		}
		
		String(String&& other) noexcept requires std::move_constructible<ALLOCATOR> : allocator(MoveRef(other.allocator)), data(other.data), bytes(other.bytes), codePoints(other.codePoints), capacity(other.capacity) {
			other.data = nullptr;
		}
		
		String& operator=(const String& other)
		{
			bytes = 0; //reset length
			tryResize(other.bytes); //test if new string exceeds available storage
			allocator = other.allocator;
			copy(other.bytes + 1, other.data);
			bytes = other.bytes;
			codePoints = other.codePoints;
			return *this;
		}
		
		String& operator=(String&& other) noexcept requires std::move_constructible<ALLOCATOR>
		{
			//free current
			allocator = MoveRef(other.allocator);
			data = other.data;
			bytes = other.bytes;
			codePoints = other.codePoints;
			capacity = other.capacity;
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

		auto begin() noexcept { return data; }
		[[nodiscard]] auto begin() const noexcept { return data; }
		auto end() noexcept { return data + bytes + 1; }
		[[nodiscard]] auto end() const noexcept { return data + bytes + 1; }

		//Returns true if the two String's contents are the same. Comparison is case sensitive.
		template<class ALLOCATOR>
		bool operator==(const String<ALLOCATOR>& other) const {
			//Discard if Length of strings is not equal, first because it helps us discard before even starting, second because we can't compare strings of different sizes.
			if (codePoints != other.codePoints or bytes != other.bytes) return false;
			for (uint32 i = 0; i < bytes; ++i) { if (data[i] != other.data[i]) { return false; } }
			return true;
		}

		bool operator==(const char8_t* text) const {
			auto l = StringByteLength(text) - 1;
			//Discard if Length of strings is not equal, first because it helps us discard before even starting, second because we can't compare strings of different sizes.
			if (bytes != l) return false;
			for (uint32 i = 0; i < bytes; ++i) { if (data[i] != text[i]) { return false; } }
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

		//operator Range<char8_t*>() const { return Range<char8_t*>(begin(), end()); }
		operator Range<const char8_t*>() const { return Range<const char8_t*>(begin(), end()); }

		[[nodiscard]] const char8_t* c_str() const { return data; }

		//Return the length of this String.
		[[nodiscard]] uint32 GetBytes() const { return bytes + 1; }
		[[nodiscard]] uint32 GetCodepoints() const { return codePoints + 1; }
		[[nodiscard]] uint32 GetCapacity() const { return capacity; }
		//Returns whether this String is empty.
		[[nodiscard]] bool IsEmpty() const { return !bytes; }

		String& operator+=(const char8_t* string) {
			copy(string);
			return *this;
		}

		String& operator+=(Range<const char8_t*> range) {
			copy(range);
			return *this;
		}

		String& operator+=(char8_t character) {
			tryResize(1, 4);
			data[bytes++] = character; data[bytes] = u8'\0';
			++codePoints;
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
		}

		void ReplaceAll(char8_t a, char8_t with) {
			for (uint32 i = 0; i < bytes; ++i) { if (data[i] == a) { data[i] = with; } }
		}

		void Resize(const uint32 newLength) {
			tryResize(bytes - newLength, 0);
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

		/**
		 * \brief Returns an index to the last char in the string that is equal to c. If no such character is found npos() is returned.
		 * \param c Char to Find.
		 * \return Index to found char.
		 */
		[[nodiscard]] friend Result<uint32> FindLast(const String& string, char8_t c)
		{
			for (uint32 i = string.GetBytes(); i < string.GetBytes(); --i) { if (string.data[i] == c) { return Result(MoveRef(i), true); } }
			return Result<uint32>(false);
		}

		/**
		* \brief Returns an index to the first char in the string that is equal to c. If no such character is found npos() is returned.
		* \param c Char to Find.
		* \return Index to found char.
		*/
		friend Result<uint32> FindFirst(const String& string, char8_t c) {
			for (uint32 i = 0; i < string.GetBytes(); ++i) { if (string.data[i] == c) { return Result(MoveRef(i), true); } }
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

		void tryResize(int64 delta, uint8 resLen)
		{
			if (bytes + delta + resLen > capacity) {
				delta += resLen;

				uint64 allocatedSize = 0; void* newData = nullptr;
				
				if(data) {
					allocator.Allocate((bytes + delta) * 2, 16, &newData, &allocatedSize);
					for (uint32 i = 0; i < bytes; ++i) { static_cast<char8_t*>(newData)[i] = data[i]; }
					allocator.Deallocate(bytes, 16, static_cast<void*>(data));
				} else {
					allocator.Allocate(bytes + delta, 16, &newData, &allocatedSize);
				}

				capacity = static_cast<uint32>(allocatedSize / sizeof(char8_t));
				data = static_cast<char8_t*>(newData);
			}
		}
		
		void copy(const Range<const char8_t*> string) {
			auto sizes = StringLengths2(string);
			uint8 resLen = uint8(4) - Get<2>(sizes);
			tryResize(Get<0>(sizes), resLen);
			for(uint32 i = 0; i < Get<0>(sizes); ++i) { data[bytes + i] = string[i]; }
			bytes += Get<0>(sizes) - 1;
			codePoints += Get<1>(sizes) - 1;
			for (uint8 i = 0; i < resLen; ++i) { data[bytes + i] = u8'\0'; }
		}

		void copy(const string_type* string) {
			auto sizes = StringLengths2(string);
			uint8 resLen = uint8(4) - Get<2>(sizes);
			tryResize(Get<0>(sizes), resLen);
			for(uint32 i = 0; i < Get<0>(sizes); ++i) { data[bytes + i] = string[i]; }
			bytes += Get<0>(sizes) - 1;
			codePoints += Get<1>(sizes) - 1;
			for (uint8 i = 0; i < resLen; ++i) { data[bytes + i] = u8'\0'; }
		}

		friend class String;

	public:
		friend void Insert(const String& string, auto& buffer) {
			Insert(string.GetBytes(), buffer);
			buffer.CopyBytes(string.GetBytes(), reinterpret_cast<const byte*>(string.begin()));
		}
		
		friend void Extract(String& string, auto& buffer) {
			uint32 bytes = 0, codepoints = 0;
			Extract(bytes, buffer); Extract(codepoints, buffer);
			string.Resize(bytes);
			buffer.CopyBytes(bytes, reinterpret_cast<const byte*>(string.begin()));
			string.bytes = bytes;
			string.codePoints = codepoints;
		}
	};

	template<uint64 N>
	using StaticString = String<StaticAllocator<N>>;

	template<class ALLOC, uint64 N>
	using SemiString = String<DoubleAllocator<N, ALLOC>>;
}
