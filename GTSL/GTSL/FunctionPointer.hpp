#pragma once

#include "Core.h"

namespace GTSL
{
	template<typename T>
	class FunctionPointer;

	template<typename RET, typename... ARGS>
	class FunctionPointer<RET(ARGS...)> final
	{
		RET(*callerFunction)(void*, ARGS&& ...) { nullptr };

	public:
		constexpr explicit FunctionPointer(decltype(callerFunction) cF) : callerFunction(cF)
		{
		}

		template <RET(*FUNCTION)(ARGS ...)>
		static constexpr RET functionCaller(void* callee, ARGS&&... params) { return (FUNCTION)(GTSL::MakeForwardReference<ARGS>(params)...); }

		template <class T, RET(T::* METHOD)(ARGS ...)>
		static constexpr RET methodCaller(void* callee, ARGS&&... params) { return (static_cast<T*>(callee)->*METHOD)(GTSL::MakeForwardReference<ARGS>(params)...); }

		template <class T, RET(T::* CONST_METHOD)(ARGS ...) const>
		static constexpr RET constMethodCaller(void* callee, ARGS&&... params) { return (static_cast<const T*>(callee)->*CONST_METHOD)(GTSL::MakeForwardReference<ARGS>(params)...); }

		template <RET(* FUNCTION)(ARGS...)>
		static constexpr FunctionPointer Create() { return FunctionPointer(functionCaller<FUNCTION>); }
		
		//template <class T, RET(T::* METHOD)(ARGS...)>
		//static constexpr FunctionPointer Create() { return FunctionPointer(methodCaller<T, METHOD>); }

		template <class T, RET(T::* CONST_METHOD)(ARGS...) const>
		static constexpr FunctionPointer Create() { return FunctionPointer(constMethodCaller<T, CONST_METHOD>); }

		template<typename T>
		constexpr RET operator()(T* callee, ARGS... args) const { return callerFunction((void*)callee, GTSL::MakeForwardReference<ARGS>(args)...); }
	};

	//template<typename T, typename RET, typename... ARGS>
	//static constexpr FunctionPointer<RET(ARGS...)> CreateFunctionPointer(RET(T::* METHOD)(ARGS...)) { return FunctionPointer<RET(ARGS...)>(FunctionPointer<RET(ARGS...)>::template methodCaller<T, METHOD>); }
	//
	//template<typename T, typename RET, typename... ARGS>
	//static constexpr FunctionPointer<RET(ARGS...)> CreateFunctionPointer(RET(T::* CONST_METHOD)(ARGS...) const) { return FunctionPointer<RET(ARGS...)>(FunctionPointer<RET(ARGS...)>::template constMethodCaller<T, CONST_METHOD>); }
}