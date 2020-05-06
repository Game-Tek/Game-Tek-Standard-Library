#pragma once

#include "Array.hpp"

namespace GTSL
{
	template<uint32 N, typename CL = UTF8>
	class StaticString
	{
		Array<CL, N> array;

		static constexpr uint32 stringLength(const char* text)
		{
			uint32 i{ 0 }; while (text[i] != '\0') { ++i; } return i + 1;
		}
	public:
		constexpr StaticString() noexcept = default;

		constexpr StaticString(const char* cstring) : array(stringLength(cstring), cstring)
		{
			GTSL_ASSERT(stringLength(cstring) > N, "Input string longer than static string size!");
		}

		[[nodiscard]] CL* begin() { return this->array.begin(); }
		[[nodiscard]] const CL* begin() const { return this->array.begin(); }
		[[nodiscard]] CL* end() { return this->array.end(); }
		[[nodiscard]] const CL* end() const { return this->array.end(); }

		[[nodiscard]] constexpr uint32 GetLength() const { return this->array.GetLength(); }
		[[nodiscard]] constexpr uint32 GetCapacity() const { return N; }

		void Resize(uint32 newLength) { this->array.Resize(newLength); }
		
		GTSL::Ranger<CL> GetRanger() { return array.GetRanger(); }
		operator GTSL::Ranger<CL>() { return array; }

		StaticString& operator+=(const char* cstring)
		{
			this->array.PopBack();
			this->array.PushBack(stringLength(cstring), cstring);
			return *this;
		}

		void Drop(const uint32 from)
		{
			this->array.Resize(from + 1);
			this->array[from + 1] = '\0';
		}

		void ReplaceAll(char a, char with)
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

		constexpr StaticString& operator<<(const char* text)
		{
			GTSL_ASSERT(stringLength(text) + this->array.GetLength() > N, "CString does not fit in static string.")
				for (uint32 i = 0; i < stringLength(text); ++i) { this->array[i] = text[i]; }
			return *this;
		}
	};
}
