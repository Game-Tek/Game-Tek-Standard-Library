#pragma once

#include "Array.hpp"

#include "StringCommon.h"

namespace GTSL
{
	template<uint32 N>
	class StaticString
	{
	public:
		constexpr StaticString() noexcept = default;

		template<uint32 NN>
		constexpr StaticString(const StaticString<NN>& other) noexcept : array(other.array) {}
		
		constexpr StaticString(const char* string) : array(Ranger<const UTF8>(StringLength(string), string)) { this->array.PopBack(); }

		constexpr StaticString(const Ranger<UTF8>& ranger) : array(ranger) {}

		constexpr StaticString(const Ranger<const UTF8>& ranger) : array(ranger) {}

		[[nodiscard]] UTF8* begin() { return this->array.begin(); }
		[[nodiscard]] UTF8* end() { return this->array.end(); }
		[[nodiscard]] const UTF8* begin() const { return this->array.begin(); }
		[[nodiscard]] const UTF8* end() const { return this->array.end(); }

		[[nodiscard]] constexpr uint32 GetLength() const { return this->array.GetLength(); }
		[[nodiscard]] constexpr uint32 GetCapacity() const { return N; }

		void Resize(uint32 newLength) { this->array.Resize(newLength); }

		operator GTSL::Ranger<const UTF8>() const { return array; }

		void Drop(const uint32 from) { this->array.Resize(from); }
		void KeepFrom(const uint32 from) { this->array.Remove(0, from); }

		void ReplaceAll(UTF8 a, const UTF8 with)
		{
			for (auto& c : this->array) { if (c == a) { c = with; } }
		}

		[[nodiscard]] uint32 npos() const { return this->array.GetLength() + 1; }

		unsigned	 FindLast(UTF8 c) const
		{
			uint32 i{ this->GetLength() };
			for (auto begin = this->array.end(); begin != this->array.begin(); --begin) { if (*begin == c) { return i; } --i; }
			return npos();
		}

		constexpr StaticString& operator+=(const char* cstring) noexcept
		{
			this->array.PushBack(Ranger<const UTF8>(StringLength(cstring), cstring)); this->array.PopBack(); return *this;
		}

		constexpr StaticString& operator+=(const Ranger<const UTF8>& ranger)
		{
			this->array.PushBack(ranger); return *this;
		}
		
		constexpr StaticString& operator+=(const UTF8 num) { this->array.PushBack(num); return *this; }

		constexpr StaticString& operator+=(const uint8 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.begin() + this->array.GetCapacity());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const int16 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.begin() + this->array.GetCapacity());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const uint16 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.begin() + this->array.GetCapacity());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const int32 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.begin() + this->array.GetCapacity());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const uint32 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.begin() + this->array.GetCapacity());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const int64 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.begin() + this->array.GetCapacity());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const uint64 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.begin() + this->array.GetCapacity());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const float32 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.begin() + this->array.GetCapacity());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

		constexpr StaticString& operator+=(const float64 num)
		{
			Ranger<UTF8> range(this->array.begin() + this->array.GetLength(), this->array.begin() + this->array.GetCapacity());
			ToString(num, range);
			this->array.PushBack(range);
			return *this;
		}

	private:
		Array<UTF8, N> array;
		friend class StaticString;
	};
}
