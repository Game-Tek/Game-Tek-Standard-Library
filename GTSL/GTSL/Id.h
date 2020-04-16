#pragma once

#include "Core.h"

#include "String.hpp"
#include "Ranger.h"

namespace GTSL
{
	class String;

	class Id64
	{
	public:
		using HashType = uint64;

	protected:
		static constexpr HashType hashString(const uint32 length, const char* text) noexcept
		{
			HashType primaryHash(525201411107845655ull);
			HashType secondaryHash(0xAAAAAAAAAAAAAAAAull);

			for (auto& c : Ranger(length, text))
			{
				primaryHash ^= c;
				secondaryHash ^= c;
				primaryHash *= 0x5bd1e9955bd1e995;
				secondaryHash *= 0x80638e;
				primaryHash ^= primaryHash >> 47;
				secondaryHash ^= secondaryHash >> 35;
			}

			//primaryHash ^= secondaryHash + 0x9e3779b9 + (primaryHash << 6) + (primaryHash >> 2);

			return ((primaryHash & 0xFFFFFFFF00000000ull) ^ (secondaryHash & 0x00000000FFFFFFFFull));
		}
	public:		
		
		constexpr static HashType HashString(const char* text) noexcept { return hashString(String::StringLength(text) - 1, text); };
		
		constexpr Id64() = default;
		
		constexpr Id64(const char* cstring) noexcept : hashValue(HashString(cstring))
		{
		}
		
		constexpr explicit Id64(HashType id) noexcept;
		explicit Id64(const String& string);
		constexpr Id64(const Id64& other) = default;
		constexpr Id64(Id64&& other) noexcept : hashValue(other.hashValue) { other.hashValue = 0; }

		~Id64() noexcept = default;

		constexpr Id64& operator=(const Id64& other) noexcept = default;
		constexpr bool operator==(const Id64& other) const noexcept { return hashValue == other.hashValue; }
		constexpr Id64& operator=(Id64&& other) noexcept { hashValue = other.hashValue; other.hashValue = 0; return *this; }

		constexpr HashType GetID() noexcept { return hashValue; }
		[[nodiscard]] constexpr HashType GetID() const noexcept { return hashValue; }

		constexpr operator HashType() const { return hashValue; }

		static HashType HashString(const String& string) noexcept;

	private:
		HashType hashValue = 0;
	};

	class Id32
	{
		uint32 hash = 0;
		constexpr static uint32 hashString(uint32 stringLength, const char* str) noexcept;
	public:
		constexpr Id32(const char* text) noexcept;
		constexpr Id32(uint32 length, const char* text) noexcept;

		constexpr operator uint32() const noexcept { return hash; }
	};

	class Id16
	{
		uint16 hash = 0;
		static uint16 hashString(uint32 stringLength, const char* str);
	public:
		Id16(const char* text);

		operator uint16() const { return hash; }
	};
}
