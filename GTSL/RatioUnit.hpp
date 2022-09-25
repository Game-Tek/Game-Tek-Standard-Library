#pragma once

#include "Core.h"

namespace GTSL
{
	template<typename ST, uint64 NUM, uint64 DEN>
	class RatioUnit
	{
	public:
		using storing_type = ST;

		static constexpr ST NUMERATOR = NUM;
		static constexpr ST DENOMINATOR = DEN;

		RatioUnit() = default;

		explicit constexpr RatioUnit(const storing_type time) : count(time) {}

		template<typename RET, class R>
		constexpr RET As() const {
			if constexpr (R::DENOMINATOR > DENOMINATOR) {
				if constexpr (NUMERATOR == R::NUMERATOR) {
					return count * (static_cast<RET>(R::DENOMINATOR) / static_cast<RET>(DENOMINATOR));
				} else {
					return count * static_cast<RET>(R::NUMERATOR / R::DENOMINATOR);
				}
			} else if constexpr (R::DENOMINATOR < DENOMINATOR) {
				if constexpr (NUMERATOR == R::NUMERATOR) {
					return count / static_cast<RET>(DENOMINATOR);
				} else {
					return count / static_cast<RET>(NUMERATOR / DENOMINATOR);
				}
			} else {
				if constexpr (R::NUMERATOR < NUMERATOR) {
					return count * static_cast<RET>(NUMERATOR);
				} else{
					return count;
				}

				return static_cast<RET>(count);
			}

			return 0;

			//auto value = static_cast<RET>(count);
			//value *= (static_cast<RET>(NUM) / static_cast<RET>(DEN));
			//return value / (static_cast<RET>(R::NUMERATOR) / static_cast<RET>(R::DENOMINATOR));
		}

		template<uint64 N, uint64 D>
		constexpr RatioUnit(RatioUnit<ST, N, D> other) : count(other.template As<ST, RatioUnit<ST, NUM, DEN>>()) {}

		constexpr explicit operator storing_type() const { return count; }

		constexpr storing_type GetCount() const { return count; }

		constexpr RatioUnit operator+(const RatioUnit& other) const { return RatioUnit(this->count + other.count); }
		constexpr RatioUnit operator-(const RatioUnit& other) const { return RatioUnit(this->count - other.count); }
		constexpr RatioUnit operator*(const RatioUnit& other) const { return RatioUnit(this->count * other.count); }
		constexpr RatioUnit operator/(const RatioUnit& other) const { return RatioUnit(this->count / other.count); }

		constexpr RatioUnit& operator+=(const RatioUnit& other) { this->count += other.count; return *this; }
		constexpr RatioUnit& operator-=(const RatioUnit& other) { this->count -= other.count; return *this; }
		constexpr RatioUnit& operator*=(const RatioUnit& other) { this->count *= other.count; return *this; }
		constexpr RatioUnit& operator/=(const RatioUnit& other) { this->count /= other.count; return *this; }

		constexpr bool operator>(const RatioUnit& other) const { return this->count > other.count; }
		constexpr bool operator<(const RatioUnit& other) const { return this->count < other.count; }

		constexpr bool operator==(const RatioUnit& other) const { return this->count == other.count; }

	protected:
		ST count{ 0 };
	};
}
