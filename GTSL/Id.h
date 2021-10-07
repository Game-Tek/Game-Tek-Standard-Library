#pragma once

#include "Core.h"

#include "Range.hpp"
#include "StringCommon.h"

namespace GTSL
{
	constexpr uint64 Hash(const Range<const char8_t*> string) {
		uint64 primary_hash(525201411107845655ull);

		for (uint32 i = 0; i < string.GetBytes(); ++i) {
			primary_hash ^= string[i]; primary_hash *= 0x5bd1e9955bd1e995; primary_hash ^= primary_hash >> 47;
		}

		return primary_hash;
	}

	class Id64
	{
	public:
		using HashType = uint64;

		constexpr Id64() = default;

		template<uint64 N>
		constexpr Id64(const char8_t(&string)[N]) noexcept : hashValue(Hash(GTSL::Range<const char8_t*>(string))) {}

		//template<char8_t... str>
		//constexpr Id64() noexcept : hashValue(hashString<str...>()) {}
		
		constexpr Id64(const Range<const char8_t*>& ranger) noexcept : hashValue(Hash(ranger)) {}
		constexpr Id64(const char8_t* text) noexcept : hashValue(Hash(text)) {}
		constexpr Id64(const Id64& other) noexcept = default;
		constexpr explicit Id64(uint64 value) noexcept : hashValue(value) {}

		~Id64() noexcept = default;

		constexpr Id64& operator=(const Id64& other) noexcept = default;
		constexpr bool operator==(const Id64& other) const noexcept { return hashValue == other.hashValue; }
		constexpr Id64& operator=(Id64&& other) noexcept { hashValue = other.hashValue; other.hashValue = 0; return *this; }

		[[nodiscard]] constexpr HashType GetID() noexcept { return hashValue; }
		[[nodiscard]] constexpr HashType GetID() const noexcept { return hashValue; }

		constexpr HashType operator()() const { return hashValue; }
		explicit constexpr operator HashType() const { return hashValue; }
	private:
		HashType hashValue = 0;
	};

	class Id32
	{
		uint32 hash = 0;
		//constexpr static uint32 hashString(uint32 stringLength, const char* str) noexcept;
	public:
		constexpr Id32(const char* text) noexcept;
		constexpr Id32(uint32 length, const char* text) noexcept;

		constexpr operator uint32() const noexcept { return hash; }
	};

	//inline constexpr uint64 operator""_hash(const char8_t* text, size_t length) { return Id64((length, text)).GetID(); }
}
