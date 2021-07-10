#pragma once

#include "Assert.h"
#include "Pair.h"
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
		
		constexpr StaticString(const char8_t character)
		{
			GTSL_ASSERT(1 <= N, "String larger than buffer capacity");
			copy(1, &character); this->length = 1;
			array[length] = '\0';
		}

		constexpr StaticString(const char8_t* string)
		{
			const auto len = StringLength(string);
			GTSL_ASSERT(len <= N, "String larger than buffer capacity");
			copy(len, string); this->length = len - 1;
		}

		constexpr StaticString(const Range<const char8_t*> ranger)
		{
			GTSL_ASSERT(ranger.ElementCount() <= N, "String larger than buffer capacity")
			copy(ranger.ElementCount(), ranger.begin()); this->length = ranger.ElementCount() - 1;
		}

		StaticString& operator=(const StaticString& other) {
			length = other.length;
			for (uint32 i = 0; i < length + 1; ++i) { array[i] = other.array[i]; }
			return *this;
		}
		
		[[nodiscard]] char8_t* begin() { return this->array; }
		[[nodiscard]] char8_t* end() { return this->array + this->length + 1; }
		[[nodiscard]] const char8_t* begin() const { return this->array; }
		[[nodiscard]] const char8_t* end() const { return this->array + this->length + 1; }

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

		constexpr operator GTSL::Range<const char8_t*>() const { return Range<const char8_t*>(this->length + 1, this->array); }

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

		void ReplaceAll(char8_t a, const char8_t with)
		{
			for (auto& c : this->array) { if (c == a) { c = with; } }
		}

		bool Find(const char8_t* string) const
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
		
		[[nodiscard]] Result<Pair<uint32, uint32>> FindLast(char8_t c) const {
			for (uint32 i = length, t = 0; t < length; --i, ++t) { if (array[i] == c) { return Result(Pair(t, i), true); } }
			return GTSL::Result<Pair<uint32, uint32>>(false);
		}

		constexpr StaticString& operator+=(const char8_t* cstring) noexcept {
			auto len = StringLength(cstring);
			GTSL_ASSERT(len <= N, "String larger than buffer capacity")
			copy(len, cstring);
			this->length += len - 1;
			return *this;
		}

		constexpr StaticString& operator+=(const Range<const char8_t*> range)
		{
			GTSL_ASSERT(range.ElementCount() <= N, "String larger than buffer capacity")
			const auto len = range.ElementCount();
			copy(len, range.begin());
			this->length += static_cast<uint32>(len - 1);
			return *this;
		}
		
		constexpr StaticString& operator+=(const char8_t num)
		{
			GTSL_ASSERT(this->length + 1 <= N, "String larger than buffer capacity")
			this->array[this->length] = num;
			++this->length;
			this->array[this->length] = '\0';
			return *this;
		}

		char8_t operator[](const uint32 index) const { return array[index]; }
		
	private:
		char8_t array[N];
		uint32 length = 0;
		friend class StaticString;

		constexpr void copy(const uint64 size, const char8_t* data)
		{
			for(uint64 i = this->length, iD = 0; iD < size; ++i, ++iD) { this->array[i] = data[iD]; }
		}
	};
}
