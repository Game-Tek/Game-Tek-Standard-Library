#pragma once

namespace GTSL
{
	template<typename R, typename E = bool>
	class Result
	{
	public:
		Result(R&& result, E err) : result(result), error(err) {}
		Result(E err) : result(), error(err) {}

		operator bool() const { return error; }
		const R& Get() const { return result; }
	private:
		R result;
		E error;
	};
}
