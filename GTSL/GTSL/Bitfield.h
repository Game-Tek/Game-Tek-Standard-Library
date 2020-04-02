#pragma once

#include "Core.h"

template<typename T>
class Bitfield
{
	T number{0};
	
	friend class BitReference;
	class BitReference
	{
		constexpr Bitfield& bitfield;
		constexpr uint8 bit;

		BitReference(Bitfield& bitfield, const uint8 bit) noexcept: bitfield(bitfield), bit(bit) {}
	public:
		BitReference& operator=(const bool b) const noexcept { bitfield.number = (bitfield.number & ~(1ull << bit)) | (b << bit); return *this; }

		operator bool() const noexcept { return (bitfield.number >> bit) & 1; }

		void Flip() const noexcept { bitfield.number ^= 1ull << bit; }
	};
	
public:
	Bitfield() = default;

	void Flip(const uint8 i) { number ^= 1ull << i; }
	void Set(const uint8 i, const bool value) { number = (number & ~(1ull << i)) | (value << i); }
	void Get(const uint8 i, bool& value) { value = ((number >> i) & 1ull); }
	
	BitReference operator[](const uint8 i) const { return BitReference(*this, i); }
	bool operator()(const uint8 i) const { return (number >> i) & 1ull; }

	operator T() const { return number; }
};