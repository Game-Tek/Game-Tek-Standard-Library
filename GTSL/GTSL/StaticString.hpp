#pragma once

#include "Array.hpp"

namespace GTSL
{
	template<uint32 N>
	class StaticString
	{
		Array<UTF8, N> array;

		static constexpr uint32 stringLength(const char* text) noexcept
		{
			uint32 i{ 0 }; while (text[i] != '\0') { ++i; } return i + 1;
		}
	public:
		constexpr StaticString() noexcept = default;

		constexpr StaticString(const char* cstring) : array(Ranger<UTF8>(stringLength(cstring), cstring))
		{
			GTSL_ASSERT(stringLength(cstring) > N, "Input string longer than static string size!");
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
			this->array.Resize(from + 1);
			this->array[from + 1] = '\0';
		}

		void ReplaceAll(UTF8 a, UTF8 with)
		{
			for (auto& c : this->array) { if (c == a) { c = with; } }
		}

		[[nodiscard]] uint32 npos() const { return this->array.GetLength() + 1; }

		[[nodiscard]] const char* CString() const { return this->array.GetData(); }

		uint32 FindLast(char c)
		{
			uint32 i{ 0 };
			for (auto& e : this->array) { if (e == c) { return i; } ++i; }
			return npos();
		}

		constexpr StaticString& operator+=(const char* cstring) noexcept
		{
			this->array.PushBack(Ranger<UTF8>(stringLength(cstring), cstring));
			return *this;
		}

		constexpr StaticString& operator+=(const Ranger<UTF8>& ranger)
		{
			GTSL_ASSERT(ranger.ElementCount() + this->array.GetLength() > N, "CString does not fit in static string.")
				this->array.PushBack(ranger);
			return *this;
		}
	};
}
