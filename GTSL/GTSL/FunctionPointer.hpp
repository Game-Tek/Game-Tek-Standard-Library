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

		template <RET(*FUNCTION)(ARGS ...)>
		static constexpr RET functionCaller(void* callee, ARGS&&... params) { return (FUNCTION)(GTSL::ForwardRef<ARGS>(params)...); }

		template <class T, RET(T::* METHOD)(ARGS ...)>
		static constexpr RET methodCaller(void* callee, ARGS&&... params) { return (static_cast<T*>(callee)->*METHOD)(GTSL::ForwardRef<ARGS>(params)...); }

		template <class T, RET(T::* CONST_METHOD)(ARGS ...) const>
		static constexpr RET constMethodCaller(void* callee, ARGS&&... params) { return (static_cast<const T*>(callee)->*CONST_METHOD)(GTSL::ForwardRef<ARGS>(params)...); }
	public:
		constexpr explicit FunctionPointer(decltype(callerFunction) cF) : callerFunction(cF)
		{
		}

		template <RET(*FUNCTION)(ARGS...)>
		static constexpr FunctionPointer Create() { return FunctionPointer(functionCaller<FUNCTION>); }
		
		//template <class T, RET(T::* METHOD)(ARGS...)>
		//static constexpr FunctionPointer Create() { return FunctionPointer(methodCaller<T, METHOD>); }

		template <class T, RET(T::* CONST_METHOD)(ARGS...) const>
		static constexpr FunctionPointer Create() { return FunctionPointer(constMethodCaller<T, CONST_METHOD>); }

		template<typename T>
		constexpr RET operator()(T* callee, ARGS... args) const { return callerFunction(static_cast<void*>(callee), GTSL::ForwardRef<ARGS>(args)...); }
	};
}