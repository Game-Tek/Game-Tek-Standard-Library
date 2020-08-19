#pragma once

#include "Assert.h"
#include "StringCommon.h"

namespace GTSL
{
	template<uint32 N>
	class StaticString
	{
	public:
		constexpr StaticString() noexcept = default;

		template<uint32 NN>
		constexpr StaticString(const StaticString<NN>& other) noexcept
		{
			copy(other.length + 1, other.array);
			this->length = other.length;
		}
		
		constexpr StaticString(const char* string)
		{
			const auto len = StringLength(string);
			GTSL_ASSERT(len <= N, "String larger than buffer capacity");
			copy(len, string); this->length = len - 1;
		}

		constexpr StaticString(const Ranger<const UTF8>& ranger)
		{
			GTSL_ASSERT(ranger.ElementCount() <= N, "String larger than buffer capacity")
			copy(ranger.ElementCount(), ranger.begin()); this->length = ranger.ElementCount() - 1;
		}

		[[nodiscard]] UTF8* begin() { return this->array; }
		[[nodiscard]] UTF8* end() { return this->array + this->length + 1; }
		[[nodiscard]] const UTF8* begin() const { return this->array; }
		[[nodiscard]] const UTF8* end() const { return this->array + this->length + 1; }

		[[nodiscard]] constexpr uint32 GetLength() const { return this->length + 1; }
		[[nodiscard]] constexpr uint32 GetCapacity() const { return N; }

		void Resize(uint32 newLength)
		{
			GTSL_ASSERT(newLength <= N, "New size larger tnah capacity")
			this->length = newLength;
		}

		operator GTSL::Ranger<const UTF8>() const { return Ranger<const UTF8>(this->length + 1, this->array); }

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

		[[nodiscard]] uint32 npos() const { return this->length + 2; }

		[[nodiscard]] unsigned FindLast(UTF8 c) const
		{
			uint32 i{ this->GetLength() };
			for (auto begin = this->end(); begin != this->begin(); --begin) { if (*begin == c) { return i; } --i; }
			return npos();
		}

		constexpr StaticString& operator+=(const char* cstring) noexcept
		{
			auto len = StringLength(cstring);
			GTSL_ASSERT(len <= N, "String larger than buffer capacity")
			copy(len, cstring);
			this->length += len - 1;
			return *this;
		}

		constexpr StaticString& operator+=(const Ranger<const UTF8>& ranger)
		{
			GTSL_ASSERT(ranger.ElementCount() <= N, "String larger than buffer capacity")
			const auto len = ranger.ElementCount();
			copy(len, ranger.begin());
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
			Ranger<UTF8> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const int16 num)
		{
			Ranger<UTF8> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const uint16 num)
		{
			Ranger<UTF8> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const int32 num)
		{
			Ranger<UTF8> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const uint32 num)
		{
			Ranger<UTF8> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const int64 num)
		{
			Ranger<UTF8> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const uint64 num)
		{
			Ranger<UTF8> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const float32 num)
		{
			Ranger<UTF8> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

		constexpr StaticString& operator+=(const float64 num)
		{
			Ranger<UTF8> range(this->begin() + this->length, this->begin() + this->GetCapacity());
			ToString(num, range);
			this->length += range.ElementCount() - 1; //to string buffer filled buffer contains null terminator but when don't account for it in string implementation
			return *this;
		}

	private:
		UTF8 array[N];
		uint32 length = 0;
		friend class StaticString;

		void copy(const uint32 size, const UTF8* data)
		{
			for(uint32 i = this->length, iD = 0; iD < size; ++i, ++iD) { this->array[i] = data[iD]; }
		}
	};
}
