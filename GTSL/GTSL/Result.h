#pragma once

namespace GTSL
{
	template<typename R, typename E = bool>
	class Result
	{
	public:
		Result(R&& result, E err) : result(result), success(err) {}
		explicit Result(E err) : result(), success(err) {}

		E State() const { return success; }
		const R& Get() const { return result; }
	private:
		R result;
		E success;
	};
}
