#pragma once

#include "Core.h"

#include "Range.h"
#include "StringCommon.h"

namespace GTSL
{
	class Id64
	{
	public:
		using HashType = uint64;

	protected:
		static constexpr HashType hashString(const Range<const UTF8*>& ranger) noexcept
		{
			HashType primary_hash(525201411107845655ull);
			HashType secondary_hash(0xAAAAAAAAAAAAAAAAull);

			for (uint32 i = 0; i < ranger.ElementCount() - 1; ++i)
			{
				primary_hash ^= ranger[i];			secondary_hash ^= ranger[i];
				primary_hash *= 0x5bd1e9955bd1e995; secondary_hash *= 0x80638e;
				primary_hash ^= primary_hash >> 47; secondary_hash ^= secondary_hash >> 35;
			}

			return (primary_hash & 0xFF00FF00FF00FF00ull) ^ (secondary_hash & 0x00FF00FF00FF00FFull);
		}
		
	public:
		constexpr Id64() = default;

		template<uint64 N>
		constexpr Id64(const char(&string)[N]) noexcept : hashValue(hashString(GTSL::Range<const UTF8*>(N, string))) {}
		
		constexpr Id64(const Range<const UTF8*>& ranger) noexcept : hashValue(hashString(ranger)) {}
		constexpr Id64(const char* text) noexcept : hashValue(hashString(GTSL::Range<const UTF8*>(StringLength(text), text))) {}
		constexpr Id64(const Id64& other) noexcept = default;
		constexpr Id64(Id64&& other) noexcept : hashValue(other.hashValue) { other.hashValue = 0; }
		constexpr explicit Id64(uint64 value) noexcept : hashValue(value) {}

		~Id64() noexcept = default;

		constexpr Id64& operator=(const Id64& other) noexcept = default;
		constexpr bool operator==(const Id64& other) const noexcept { return hashValue == other.hashValue; }
		constexpr Id64& operator=(Id64&& other) noexcept { hashValue = other.hashValue; other.hashValue = 0; return *this; }

		[[nodiscard]] constexpr HashType GetID() noexcept { return hashValue; }
		[[nodiscard]] constexpr HashType GetID() const noexcept { return hashValue; }

		[[nodiscard]] constexpr operator HashType() const noexcept { return hashValue; }
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
}
