#pragma once

#include "Array.hpp"

namespace GTSL
{
	template<uint32 N, typename CL = UTF8>
	class StaticString
	{
		Array<CL, N> array;

		static constexpr uint32 stringLength(const char* text) noexcept
		{
			uint32 i{ 0 }; while (text[i] != '\0') { ++i; } return i + 1;
		}
	public:
		constexpr StaticString() noexcept = default;

		constexpr StaticString(const char* cstring) : array(Ranger<CL>(stringLength(cstring), cstring))
		{
			GTSL_ASSERT(stringLength(cstring) > N, "Input string longer than static string size!");
		}

		constexpr StaticString(const Ranger<CL>& ranger) : array(ranger)
		{
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

		constexpr StaticString& operator+=(const char* cstring) noexcept
		{
			this->array.PopBack();
			this->array.PushBack(Ranger<CL>(stringLength(cstring), cstring));
			return *this;
		}
		
		constexpr StaticString& operator+=(const Ranger<UTF8>& ranger)
		{
			GTSL_ASSERT(ranger.ElementCount() + this->array.GetLength() > N, "CString does not fit in static string.")
			this->array.PopBack();
			this->array.PushBack(ranger);
			return *this;
		}
	};
}
