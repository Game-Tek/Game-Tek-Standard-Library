#pragma once

#include "Core.hpp"
#include "String/StringCommon.hpp"

namespace GTSL
{
	class Id
	{
	public:
		constexpr Id() = default;

		template<uint64 N>
		constexpr Id(const char* (&string)[N]) noexcept : m_hashValue(Hash(StringView(string))) {}
		constexpr Id(const char8_t* (&string)[N]) noexcept : m_hashValue(Hash(StringView(string))) {}
		constexpr Id(const StringView ranger) noexcept : m_hashValue(Hash<StringView>(StringView(ranger))) {}
		constexpr Id(const char* text) noexcept : m_hashValue(Hash<StringView>(StringView(text))) {}
		constexpr Id(const char8_t* text) noexcept : m_hashValue(Hash<StringView>(StringView(text))) {}
		constexpr Id(const Id& other) noexcept = default;
		constexpr explicit Id(uint64 value) noexcept : m_hashValue(value) {}

		~Id() noexcept = default;

		constexpr Id& operator=(const Id& other) noexcept = default;
		constexpr bool operator==(const Id& other) const noexcept { return m_hashValue == other.m_hashValue; }
		constexpr Id& operator=(Id&& other) noexcept { m_hashValue = other.m_hashValue; other.m_hashValue = 0; return *this; }

		[[nodiscard]] constexpr uint64 GetID() noexcept { return m_hashValue; }
		[[nodiscard]] constexpr uint64 GetID() const noexcept { return m_hashValue; }

		constexpr uint64 operator()() const { return m_hashValue; }
		explicit constexpr operator HashType() const { return m_hashValue; }
	private:
		uint64 m_hashValue = 0;
	};
}