#pragma once

#include "Array.hpp"

namespace GTSL
{
	template<uint32 N, typename CL = char>
	class StaticString
	{
		Array<CL, N> array;

		static constexpr uint32 stringLength(const char* text)
		{
			uint32 i{ 0 };
			while (*text) { ++i; }
			return i + 1;
		}
	public:
		constexpr StaticString() noexcept = default;
		
		constexpr StaticString(const char* cstring) : array(stringLength(cstring), cstring)
		{
			GTSL_ASSERT(stringLength(cstring) > N, "Input string longer than static string size!");
		}

		StaticString& operator+=(const char* cstring)
		{
			array.PopBack();
			array.PushBack(stringLength(cstring), cstring);
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

		const char* CString() const { return this->array.GetData(); }
		
		uint32 FindLast(char c)
		{
			uint32 i{ 0 };
			for (auto& e : this->array) { if (e == c) { return i; } ++i; }
			return npos();
		}
	};
}
