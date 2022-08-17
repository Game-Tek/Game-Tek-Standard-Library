#pragma once

#include "Core.h"
#include "Assert.h"

#include <initializer_list>

#include "Tuple.hpp"

namespace GTSL
{
	template<typename I>
	struct Range {
		Range(I b, I e) : from(b), to(e) {}

		I& begin() { return from; }
		I& end() { return to; }

		const I& begin() const { return from; }
		const I& end() const { return to; }

		Range Reverse() const { return Range(end(), begin()); }

		using type = I;

	private:
		I from, to;

		friend struct Range;
	};

	template<typename I>
	struct Range<I*> {
		Range() = default;

		constexpr Range(std::initializer_list<I> il) : from(il.begin()), to(il.end()) {}

		constexpr Range(I* b, I* e) : from(b), to(e) {}

		constexpr Range(const uint64 length, I* b) : from(b), to(b + length) {}

		//template<uint64 N>
		//Range(char8_t const(&string)[N]) : Range(N, string) {}

		[[nodiscard]] constexpr I* begin() { return from; }
		[[nodiscard]] constexpr I* end() { return to; }

		//[[nodiscard]] I* begin() const { return from; }
		//[[nodiscard]] I* end() const { return to; }

		[[nodiscard]] Range Reverse() const { return Range(end() - 1, begin() - 1); }

		[[nodiscard]] constexpr I* begin() const { return from; }
		[[nodiscard]] constexpr I* end() const { return to; }

		using type = I;

		[[nodiscard]] constexpr uint64 Bytes() const { return (to - from) * sizeof(I); }

		[[nodiscard]] constexpr uint64 ElementCount() const noexcept { return to - from; }

		constexpr I& operator[](const uint64 i) {
			GTSL_ASSERT((this->from + i) < to, "Unbounded access!")
				return this->from[i];
		}

		constexpr I& operator[](const uint64 i) const {
			GTSL_ASSERT((this->from + i) < to, "Unbounded access!")
				return this->from[i];
		}

		constexpr operator Range<const I*>() const { return Range<const I*>(static_cast<const I*>(this->from), static_cast<const I*>(this->to)); }

		bool operator==(const Range other) const {
			if (ElementCount() != other.ElementCount()) { return false; }
			for (uint64 i = 0; i < ElementCount(); ++i) { if (from[i] != other[i]) { return false; } }
			return true;
		}
	private:
		I* from = nullptr, * to = nullptr;

		friend struct Range;
	};

	template<typename... TYPES>
	struct MultiRange {
	public:
		MultiRange(const uint32 len, TYPES*... args) : pointers(GTSL::ForwardRef<TYPES*>(args)...), length(len) {
		}

		template<uint32 M>
		auto GetPointer(const uint32 index) -> typename TypeAt<M, TYPES...>::type* {
			return GTSL::Get<M>(pointers) + index;
		}

		template<uint32 M>
		auto Get(const uint32 index) -> typename TypeAt<M, TYPES...>::type {
			return *GetPointer<M>(index);
		}

		uint32 GetLength() const { return length; }

	private:
		Tuple<TYPES*...> pointers;
		uint32 length = 0;
	};

	template<>
	struct Range<const char8_t*>;

	template<typename T>
	Range(std::initializer_list<T>) -> Range<const T*>;

	template<typename T>
	Range(uint32, T*) -> Range<const T*>;

	template<typename T>
	bool IsAnyOf(const T& a, const Range<T> params) {
		for (const auto& e : params) { if (e == a) { return true; } }
		return false;
	}
}
