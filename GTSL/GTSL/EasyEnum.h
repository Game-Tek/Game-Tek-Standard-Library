#pragma once

namespace GTSL
{
	template<typename T>
	struct EasyEnum
	{
		EasyEnum() = default;
		EasyEnum(T val) : value(val) {}

		operator T() const { return value; }
		operator T& () { return value; }

		using value_type = T;
	private:
		T value = 0;
	};
}