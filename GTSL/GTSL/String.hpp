#pragma once

#include <concepts>

#include "Core.h"
#include "StringCommon.h"
#include "Allocator.h"

namespace GTSL
{
	template<class ALLOCATOR>
	class String
	{
	public:
		using string_type = char8_t;
		
		String() = default;

		/**
		 * \brief Creates an String with enough space allocated for length elements.
		 * \param initialCapacity Amount of elements to allocate.
		 */
		String(const uint32 initialCapacity, const ALLOCATOR& allocatorReference = ALLOCATOR()) : allocator(allocatorReference)
		{
			uint64 allocatedSize = 0;
			allocator.Allocate(initialCapacity, 16, reinterpret_cast<void**>(&data), &allocatedSize);
			capacity = static_cast<uint32>(allocatedSize / sizeof(string_type));
		}
		
		String(const char8_t* cstring, const ALLOCATOR& allocatorReference = ALLOCATOR()) : String(StringLength(cstring), allocatorReference)
		{
			auto stringLength = StringLength(cstring);
			copy(stringLength, cstring);
			length = stringLength - 1;
		}

		String(const Range<const char8_t*> range, const ALLOCATOR& allocatorReference = ALLOCATOR()) : String(static_cast<uint32>(range.ElementCount()), allocatorReference)
		{
			auto stringLength = static_cast<uint32>(range.ElementCount());
			copy(stringLength, range.begin());
			length = stringLength - 1;
		}

		String(const String& other) : allocator(other.allocator)
		{
			uint64 allocatedSize = 0;
			allocator.Allocate(other.length + 1, 16, reinterpret_cast<void**>(&data), &allocatedSize);
			capacity = static_cast<uint32>(allocatedSize / sizeof(string_type));
			copy(other.length, other.data);
			length = other.length;
			data[length] = u8'\0';
		}
		
		template<class B>
		String(const String<B>& other, const ALLOCATOR& allocatorReference = ALLOCATOR()) : allocator(allocatorReference)
		{
			uint64 allocatedSize = 0;
			allocator.Allocate(other.length + 1, 16, reinterpret_cast<void**>(&data), &allocatedSize);
			capacity = static_cast<uint32>(allocatedSize / sizeof(string_type));
			copy(other.length, other.data);
			length = other.length;
			data[length] = u8'\0';
		}
		
		String(String&& other) noexcept requires std::move_constructible<ALLOCATOR> : allocator(MoveRef(other.allocator)), data(other.data), length(other.length), capacity(other.capacity) {
			other.data = nullptr;
		}
		
		String& operator=(const String& other)
		{
			length = 0; //reset length
			tryResize(other.length); //test if new string exceeds available storage
			allocator = other.allocator;
			copy(other.length + 1, other.data);
			length = other.length;
			return *this;
		}
		
		String& operator=(String&& other) noexcept requires std::move_constructible<ALLOCATOR>
		{
			//free current
			allocator = MoveRef(other.allocator);
			data = other.data;
			length = other.length;
			capacity = other.capacity;
			return *this;
		}
		
		~String() {
			if (data) { allocator.Deallocate(capacity, 16, data); }
		}

		string_type operator[](const uint32 i) const noexcept { return data[i]; }

		auto begin() noexcept { return data; }
		[[nodiscard]] auto begin() const noexcept { return data; }
		auto end() noexcept { return data + length + 1; }
		[[nodiscard]] auto end() const noexcept { return data + length + 1; }

		//Returns true if the two String's contents are the same. Comparison is case sensitive.
		template<class ALLOCATOR>
		bool operator==(const String<ALLOCATOR>& other) const
		{
			//Discard if Length of strings is not equal, first because it helps us discard before even starting, second because we can't compare strings of different sizes.
			if (length != other.length) return false;
			for (uint32 i = 0; i < length; ++i) { if (data[i] != other.data[i]) { return false; } }
			return true;
		}

		bool operator==(const char8_t* text) const
		{
			auto l = StringLength(text) - 1;
			//Discard if Length of strings is not equal, first because it helps us discard before even starting, second because we can't compare strings of different sizes.
			if (length != l) return false;
			for (uint32 i = 0; i < length; ++i) { if (data[i] != text[i]) { return false; } }
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
		[[nodiscard]] uint32 GetLength() const { return length + 1; }
		[[nodiscard]] uint32 GetCapacity() const { return capacity; }
		//Returns whether this String is empty.
		[[nodiscard]] bool IsEmpty() const { return !length; }

		String& operator+=(const char8_t* string)
		{
			auto stringLength = StringLength(string);
			tryResize(stringLength);
			copy(stringLength, string);
			length += stringLength - 1;
			return *this;
		}

		String& operator+=(Range<const char8_t*> range)
		{
			auto stringLength = static_cast<uint32>(range.ElementCount());
			tryResize(stringLength);
			copy(stringLength, range.begin());
			length += stringLength - 1;
			return *this;
		}


		String& operator+=(char8_t character)
		{
			tryResize(1);
			data[length++] = character;
			data[length] = '\0';
			return *this;
		}

		/**
		 * \brief Drops/removes the parts of the string from from forward.
		 * \param from index to cut forward from.
		 */
		void Drop(uint32 from) {
			length = from; data[from] = '\0';
		}

		void ReplaceAll(char8_t a, char8_t with)
		{
			for (uint32 i = 0; i < length; ++i) { if (data[i] == a) { data[i] = with; } }
		}

		void Resize(const uint32 newLength) {
			tryResize(length - newLength);
			length = newLength;
			data[newLength] = '\0';
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

		/**
		 * \brief Returns an index to the last char in the string that is equal to c. If no such character is found npos() is returned.
		 * \param c Char to Find.
		 * \return Index to found char.
		 */
		[[nodiscard]] friend Result<uint32> FindLast(const String& string, char8_t c)
		{
			for (uint32 i = string.length; i < string.GetLength(); --i) { if (string.data[i] == c) { return Result(MoveRef(i), true); } }
			return Result<uint32>(false);
		}

		/**
		* \brief Returns an index to the first char in the string that is equal to c. If no such character is found npos() is returned.
		* \param c Char to Find.
		* \return Index to found char.
		*/
		friend Result<uint32> FindFirst(const String& string, char8_t c) {
			for (uint32 i = 0; i < string.length; ++i) { if (string.data[i] == c) { return Result(MoveRef(i), true); } }
			return Result<uint32>(false);
		}
	
	private:
		[[no_unique_address]] ALLOCATOR allocator;
		char8_t* data = nullptr; uint32 length = 0, capacity = 0;

		void tryResize(int64 delta)
		{
			if (length + delta > capacity) {
				uint64 allocatedSize = 0; void* newData = nullptr;
				
				if(data) {
					allocator.Allocate((length + delta) * 2, 16, &newData, &allocatedSize);
					for (uint32 i = 0; i < length; ++i) { static_cast<char8_t*>(newData)[i] = data[i]; }					
					allocator.Deallocate(length, 16, static_cast<void*>(data));
				} else {
					allocator.Allocate(length + delta, 16, &newData, &allocatedSize);
				}

				capacity = static_cast<uint32>(allocatedSize / sizeof(char8_t));
				data = static_cast<char8_t*>(newData);
			}
		}
		
		void copy(uint32 stringLength, const string_type* string)
		{
			GTSL_ASSERT(stringLength + length < capacity, "Not enough space");
			for (uint32 i = 0, t = length; i < stringLength; ++i, ++t) { data[t] = string[i]; }
		}

		friend class String;
	};
	
	template<uint64 N>
	using StaticString = String<StaticAllocator<N>>;

	template<class ALLOC, uint64 N>
	using SemiString = String<DoubleAllocator<N, ALLOC>>;
}
