#pragma once

#include "Assert.h"
#include "StringCommon.h"

namespace GTSL
{
	template<uint32 N>
	class StaticString
	{
	public:
		constexpr StaticString() noexcept : length(0) { array[0] = '\0'; }

		template<uint32 NN>
		constexpr StaticString(const StaticString<NN>& other) noexcept
		{
			copy(other.length + 1, other.array);
			this->length = other.length;
		}
		
		constexpr StaticString(const char character)
		{
			GTSL_ASSERT(1 <= N, "String larger than buffer capacity");
			copy(1, &character); this->length = 1;
			array[length] = '\0';
		}

		constexpr StaticString(const char* string)
		{
			const auto len = StringLength(string);
			GTSL_ASSERT(len <= N, "String larger than buffer capacity");
			copy(len, string); this->length = len - 1;
		}

		constexpr StaticString(const Range<const UTF8*> ranger)
		{
			GTSL_ASSERT(ranger.ElementCount() <= N, "String larger than buffer capacity")
			copy(ranger.ElementCount(), ranger.begin()); this->length = ranger.ElementCount() - 1;
		}

		StaticString& operator=(const StaticString& other) {
			length = other.length;
			for (uint32 i = 0; i < length + 1; ++i) { array[i] = other.array[i]; }
			return *this;
		}
		
		[[nodiscard]] UTF8* begin() { return this->array; }
		[[nodiscard]] UTF8* end() { return this->array + this->length + 1; }
		[[nodiscard]] const UTF8* begin() const { return this->array; }
		[[nodiscard]] const UTF8* end() const { return this->array + this->length + 1; }

		[[nodiscard]] bool IsEmpty() const { return length == 0; }
		
		[[nodiscard]] constexpr uint32 GetLength() const { return this->length + 1; }
		[[nodiscard]] constexpr uint32 GetCapacity() const { return N; }

		bool operator==(const StaticString& other) const
		{
			if (this->length != other.length) { return false; }
			for (uint32 i = 0; i < this->length; ++i) { if (this->array[i] != other.array[i]) { return false; } }
			return true;
		}

		template<uint32 ON>
		bool operator==(const StaticString<ON>& other) const
		{
			if (this->length != other.length) { return false; }
			for (uint32 i = 0; i < this->length; ++i) { if (this->array[i] != other.array[i]) { return false; } }
			return true;
		}
		
		bool operator!=(const StaticString& other) const
		{
			if (this->length != other.length) { return true; }
			for (uint32 i = 0; i < this->length; ++i) { if (this->array[i] != other.array[i]) { return true; } }
			return false;
		}

		template<uint32 ON>
		bool operator!=(const StaticString<ON>& other) const
		{
			if (this->length != other.length) { return true; }
			for (uint32 i = 0; i < this->length; ++i) { if (this->array[i] != other.array[i]) { return true; } }
			return false;
		}
		
		
		void Resize(uint32 newLength)
		{
			GTSL_ASSERT(newLength <= N, "New size larger tnah capacity")
			this->length = newLength;
		}

		constexpr operator GTSL::Range<const UTF8*>() const { return Range<const UTF8*>(this->length + 1, this->array); }

		void Drop(const uint32 from)
		{
			GTSL_ASSERT(from >= 0 && from <= this->length, "Non valid range")
			this->length = from;
			this->array[from] = '\0';
		}
		
		//void KeepFrom(const uint32 from)
		//{
		//	this->array.Remove(0, from);
		//}

		void ReplaceAll(UTF8 a, const UTF8 with)
		{
			for (auto& c : this->array) { if (c == a) { c = with; } }
		}

		bool Find(const char* string) const
		{
			auto stringLength = StringLength(string) - 1;
			
			for (uint32 i = 0, a = 0; i < length; ++i) {
				while (array[i] == string[a]) {
					if (++a == stringLength) { return true; }
					++i;
				}

				a = 0;
			}

			return false;
		}
		
		[[nodiscard]] GTSL::Result<uint32> FindLast(UTF8 c) const
		{
			for (uint32 i = length, t = 0; t < length; --i, ++t) { if (array[i] == c) { return GTSL::Result<uint32>(GTSL::MoveRef(i), true); } }
			return GTSL::Result<uint32>(0, false);
		}

		constexpr StaticString& operator+=(const char* cstring) noexcept
		{
			auto len = StringLength(cstring);
			GTSL_ASSERT(len <= N, "String larger than buffer capacity")
			copy(len, cstring);
			this->length += len - 1;
			return *this;
		}

		constexpr StaticString& operator+=(const Range<const UTF8*> range)
		{
			GTSL_ASSERT(range.ElementCount() <= N, "String larger than buffer capacity")
			const auto len = range.ElementCount();
			copy(len, range.begin());
			this->length += len - 1;
			return *this;
		}
		
		constexpr StaticString& operator+=(const UTF8 num)
		{
			GTSL_ASSERT(this->length + 1 <= N, "String larger than buffer capacity")
			this->array[this->length] = num;
			++this->length;
			this->array[this->length] = '\0';
			return *this;
		}

		constexpr StaticString& operator+=(const uint8 num)
		{
			Range<UTF8*> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const int16 num)
		{
			Range<UTF8*> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const uint16 num)
		{
			Range<UTF8*> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const int32 num)
		{
			Range<UTF8*> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const uint32 num)
		{
			Range<UTF8*> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const int64 num)
		{
			Range<UTF8*> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const uint64 num)
		{
			Range<UTF8*> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const float32 num)
		{
			Range<UTF8*> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const float64 num)
		{
			Range<UTF8*> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		UTF8 operator[](const uint32 index) const { return array[index]; }
		
	private:
		UTF8 array[N];
		uint32 length = 0;
		friend class StaticString;

		constexpr void copy(const uint32 size, const UTF8* data)
		{
			for(uint32 i = this->length, iD = 0; iD < size; ++i, ++iD) { this->array[i] = data[iD]; }
		}
	};
}
