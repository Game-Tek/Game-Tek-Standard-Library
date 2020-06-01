#pragma once

#include "Array.hpp"

#include "StringCommon.h"

namespace GTSL
{
	template<uint32 N>
	class StaticString
	{
		Array<UTF8, N> array;

	public:
		constexpr StaticString() noexcept = default;

		constexpr StaticString(const char* cstring) : array(Ranger<UTF8>(StringLength(cstring), cstring))
		{
		}

		constexpr StaticString(const Ranger<UTF8>& ranger) : array(ranger)
		{
		}

		[[nodiscard]] UTF8* begin() { return this->array.begin(); }
		[[nodiscard]] const UTF8* begin() const { return this->array.begin(); }
		[[nodiscard]] UTF8* end() { return this->array.end(); }
		[[nodiscard]] const UTF8* end() const { return this->array.end(); }

		[[nodiscard]] constexpr uint32 GetLength() const { return this->array.GetLength(); }
		[[nodiscard]] constexpr uint32 GetCapacity() const { return N; }

		void Resize(uint32 newLength) { this->array.Resize(newLength); }

		GTSL::Ranger<UTF8> GetRanger() { return array.GetRanger(); }
		operator GTSL::Ranger<UTF8>() const { return array; }

		void Drop(const uint32 from)
		{
			this->array.Resize(from);
			this->array[from] = '\0';
		}

		void ReplaceAll(UTF8 a, UTF8 with)
		{
			for (auto& c : this->array) { if (c == a) { c = with; } }
		}

		[[nodiscard]] uint32 npos() const { return this->array.GetLength() + 1; }

		[[nodiscard]] const char* CString() const { return this->array.GetData(); }

		uint32 FindLast(UTF8 c)
		{
			uint32 i{ this->GetLength() };
			for (auto begin = this->array.end(); begin != this->array.begin(); --begin) { if (*begin == c) { return i; } --i; }
			return npos();
		}

		constexpr StaticString& operator+=(const char* cstring) noexcept
		{
			this->array.PushBack(Ranger<UTF8>(StringLength(cstring) - 1, cstring));
			return *this;
		}

		constexpr StaticString& operator+=(const Ranger<UTF8>& ranger)
		{
			this->array.PushBack(ranger);
			return *this;
		}

		constexpr StaticString& operator+=(const int8 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.end());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const uint8 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.end());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const int16 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.end());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const uint16 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.end());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const int32 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.end());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const uint32 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.end());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const int64 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.end());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const uint64 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.end());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const float32 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.end());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const float64 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.end());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}
	};
}
