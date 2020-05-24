#pragma once

#include "Core.h"

#include "Ranger.h"

namespace GTSL
{
	class String;

	class Id64
	{
	public:
		using HashType = uint64;

	protected:
		static constexpr HashType hashString(const Ranger<const UTF8>& ranger) noexcept
		{
			HashType primary_hash(525201411107845655ull);
			HashType secondary_hash(0xAAAAAAAAAAAAAAAAull);

			for (auto const& e : ranger)
			{
				primary_hash ^= e; secondary_hash ^= e;
				primary_hash *= 0x5bd1e9955bd1e995; secondary_hash *= 0x80638e;
				primary_hash ^= primary_hash >> 47; secondary_hash ^= secondary_hash >> 35;
			}

			return (primary_hash & 0xFFFFFFFF00000000ull) ^ (secondary_hash & 0x00000000FFFFFFFFull);
		}

		static constexpr HashType hashString(const Ranger<UTF8>& ranger) noexcept
		{
			HashType primary_hash(525201411107845655ull);
			HashType secondary_hash(0xAAAAAAAAAAAAAAAAull);

			for (auto const& e : ranger)
			{
				primary_hash ^= e; secondary_hash ^= e;
				primary_hash *= 0x5bd1e9955bd1e995; secondary_hash *= 0x80638e;
				primary_hash ^= primary_hash >> 47; secondary_hash ^= secondary_hash >> 35;
			}

			return (primary_hash & 0xFFFFFFFF00000000ull) ^ (secondary_hash & 0x00000000FFFFFFFFull);
		}
		
	public:
		constexpr Id64() = default;

		constexpr Id64(const Ranger<const UTF8>& ranger) noexcept : hashValue(hashString(ranger)) {}
		constexpr Id64(const Ranger<UTF8>& ranger) noexcept : hashValue(hashString(ranger)) {}
		constexpr Id64(const HashType id) noexcept : hashValue(id) {}
		constexpr Id64(const Id64& other) noexcept = default;
		constexpr Id64(Id64&& other) noexcept : hashValue(other.hashValue) { other.hashValue = 0; }

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
