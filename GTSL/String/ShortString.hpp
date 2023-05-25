//module;
/*
#pragma once

#include "../Core.hpp"
#include "../Collections/Range.hpp"
#include "StringCommon.hpp"

//export module ShortString;

namespace GTSL
{
	template<uint8 SIZE>
	class ShortString
	{
	public:
		static_assert(SIZE < 255, "Size must be less that 255");
		
		constexpr ShortString() {
			array[SIZE - 1] = SIZE - 1; // Set bytes left to SIZE - 1, because the last byte is used to store the length.
		}

		// template<uint8 N>
		// constexpr ShortString(const ShortString<N>& other) : ShortString() {
		// 	(*this) += other;
		// }

		// template<uint64 N>
		// constexpr ShortString(const char8_t(&string)[N]) : ShortString() {
		// 	(*this) += string;
		// }
		
		constexpr operator Range<const char8_t*>() const { return Range<const char8_t*>(array); }

		[[nodiscard]] constexpr const char8_t* begin() const { return array; }
		[[nodiscard]] constexpr const char8_t* end() const { return array + GetBytes(); }

		constexpr ShortString(const Range<const char8_t*> text) : ShortString() {
			uint64 bytesToCopy = GTSL::Math::Min((uint64)text.GetBytes(), getRemainingBytes());

			for(uint64 i = 0; i < bytesToCopy; ++i) {
				array[i] = text.GetData()[i];
			}

			array[SIZE - 1] -= bytesToCopy;

			array[bytesToCopy] = '\0';
		}

		constexpr ShortString& operator=(const Range<const char8_t*> range) {
			array[SIZE - 1] = SIZE;
			*this += range; return *this;
		}

		constexpr ShortString& operator+=(const Range<const char8_t*> text) {
			const uint64 bytesToCopy = GTSL::Math::Min((uint64)text.GetBytes(), getRemainingBytes());

			for (uint64 s = GetBytes(), i = 0; i < bytesToCopy; ++i, ++s) {
				array[s] = text.GetData()[i];
			}

			array[SIZE - 1] -= bytesToCopy;

			array[GetBytes()] = '\0';

			return *this;
		}

		template<uint8 N>
		constexpr bool operator==(const ShortString<N>& other) const {
			if (GetBytes() != other.GetBytes()) { return false; }
			for (uint16 i = 0; i < GetBytes(); ++i) { if (array[i] != other.array[i]) { return false; } }
			return true;
		}

		[[nodiscard]] constexpr uint64 GetBytes() const { return SIZE - array[SIZE - 1] - 1; }

		friend void Insert(const ShortString& string, auto& buffer) {
			buffer.Write(SIZE, reinterpret_cast<const byte*>(string.array));
		}

		friend void Extract(ShortString& string, auto& buffer) {
			buffer.Read(SIZE, reinterpret_cast<byte*>(string.array));
		}
		
	private:
		char8_t array[SIZE]{ 0 };

		uint64 getRemainingBytes() const { return array[SIZE - 1]; }

#if _WIN32
		friend class ShortString;
#endif
	};
}*/