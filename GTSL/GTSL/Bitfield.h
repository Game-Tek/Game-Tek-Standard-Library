#pragma once

#include "Core.h"

namespace GTSL
{
	template<uint16 N>
	class Bitfield
	{
		constexpr uint8 number[N / 8 + 1]{ 0 };

		friend class BitReference;
		class BitReference
		{
			Bitfield& bitfield;
			uint16 bit{ 0 };

			constexpr BitReference(Bitfield& bitfield, const uint16 bit) noexcept : bitfield(bitfield), bit(bit) {}
		public:
			constexpr BitReference& operator=(const bool b) const noexcept
			{
				uint8 num{bitfield.number[bit / 8]};
				num = (num & ~(1ull << (bit % 8))) | (b << (bit % 8));
				bitfield.number[bit / 8] = num;
				return *this;
			}

			constexpr operator bool() const noexcept { return (bitfield.number[bit / 8] >> (bit % 8)) & 1; }

			constexpr void Flip() const noexcept { bitfield.number[bit / 8] ^= 1ull << (bit % 8); }
		};

	public:
		constexpr Bitfield() noexcept = default;

		constexpr void Flip(const uint16 i) noexcept { number[i / 8] ^= 1ull << (i % 8); }
		constexpr void Set(const uint16 i, const bool value) noexcept { number[i / 8] = (number[i / 8] & ~(1ull << (i % 8))) | (value << (i % 8)); }
		constexpr void Get(const uint16 i, bool& value) noexcept { value = ((number[i / 8] >> (i % 8)) & 1ull); }

		constexpr BitReference operator[](const uint16 i) const noexcept { return BitReference(*this, i); }
		constexpr bool operator()(const uint16 i) const noexcept { return (number[i / 8] >> (i % 8)) & 1ull; }
	};
}