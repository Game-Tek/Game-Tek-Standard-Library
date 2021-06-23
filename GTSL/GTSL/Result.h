#pragma once

namespace GTSL
{
	template<typename R, typename E = bool>
	class Result
	{
	public:
		constexpr Result(R&& result, E err) : result(result), success(err) {}
		explicit Result(E err) : result(), success(err) {}

		[[nodiscard]] constexpr E State() const { return success; }
		[[nodiscard]] constexpr const R& Get() const { return result; }
	private:
		R result;
		E success;
	};

	template<typename R>
	class Result<R, bool>
	{
	public:
		constexpr Result(R&& result, bool err) : result(result), success(err) {}
		explicit Result(const bool err) : result(), success(err) {}

		[[nodiscard]] constexpr bool State() const { return success; }
		constexpr operator bool() const { return success; }
		[[nodiscard]] constexpr const R& Get() const { return result; }
	private:
		R result;
		bool success;
	};
}
