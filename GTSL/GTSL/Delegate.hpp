#pragma once

/*
	Copyright (C) 2017 by Sergey A Kryukov: derived work
	http://www.SAKryukov.org
	http://www.codeproject.com/Members/SAKryukov
	Based on original work by Sergey Ryazanov:
	"The Impossibly Fast C++ Delegates", 18 Jul 2005
	https://www.codeproject.com/articles/11015/the-impossibly-fast-c-delegates
	MIT license:
	http://en.wikipedia.org/wiki/MIT_License
	Original publication: https://www.codeproject.com/Articles/1170503/The-Impossibly-Fast-Cplusplus-Delegates-Fixed
*/

#include "Core.h"

namespace GTSL
{
	template <typename T>
	class Delegate;

	template <typename RET, typename... ARGS>
	class Delegate<RET(ARGS...)> final
	{
	public:
		using call_signature = RET(*)(ARGS...);

		using return_type = RET;
		
		Delegate() = default;
		~Delegate() = default;

		Delegate(const Delegate& another) noexcept : callerFunction(another.callerFunction), callee(another.callee)
		{
		}

		Delegate(Delegate&& another) noexcept : callerFunction(another.callerFunction), callee(another.callee)
		{
			another.callee = nullptr; another.callerFunction = nullptr;
		}

		//template <RET(*FUNCTION)(PARAMS ...)>
		//explicit Delegate() : callerFunction(&functionCaller<FUNCTION>), callee(nullptr)
		//{}

		//template <class T, RET(T::*METHOD)(PARAMS...)>
		//explicit Delegate(T* instance) : callerFunction(&methodCaller<T, METHOD>), callee(instance)
		//{}
		//
		//template <class T, RET(T::*CONST_METHOD)(PARAMS...) const>
		//explicit Delegate(T const* instance) : callerFunction(&constMethodCaller<T, CONST_METHOD>), callee(const_cast<T*>(instance))
		//{}
		//
		//template <typename LAMBDA>
		//explicit Delegate(LAMBDA& lambda) : callerFunction(&lambdaCaller<LAMBDA>), callee(reinterpret_cast<void*>(&lambda))
		//{}

		operator bool() const noexcept { return callerFunction; }

		Delegate& operator=(const Delegate& another)
		{
			callee = another.callee; callerFunction = another.callerFunction;
			return *this;
		}
		
		Delegate& operator=(Delegate&& another) noexcept
		{
			callee = another.callee; callerFunction = another.callerFunction;
			another.callee = nullptr; another.callerFunction = nullptr;
			return *this;
		}

		template <typename LAMBDA> // template instantiation is not needed, will be deduced (inferred):
		Delegate& operator=(const LAMBDA& instance) { callee = static_cast<void*>(&instance); callerFunction = lambdaCaller<LAMBDA>; return *this; }

		bool operator ==(const Delegate& another) const { return callerFunction == another.callerFunction && callee == another.callee; }
		bool operator !=(const Delegate& another) const { return callerFunction != another.callerFunction && callee != another.callee; }

		template <class T, RET(T::* METHOD)(ARGS...)>
		static Delegate Create(T* instance) { return Delegate(instance, methodCaller<T, METHOD>); }

		template <class T, RET(T::* CONST_METHOD)(ARGS...) const>
		static Delegate Create(T const* instance) { return Delegate(const_cast<T*>(instance), constMethodCaller<T, CONST_METHOD>); }

		template <RET(*FUNCTION)(ARGS...)>
		static Delegate Create() { return Delegate(nullptr, functionCaller<FUNCTION>); }

		template <typename LAMBDA>
		static Delegate Create(LAMBDA& instance) { return Delegate(static_cast<void*>(&instance), lambdaCaller<LAMBDA>); }

		RET operator()(ARGS... args) const { return (*callerFunction)(callee, GTSL::ForwardRef<ARGS>(args)...); }

	private:		
		RET(*callerFunction)(void*, ARGS&&...) { nullptr };
		void* callee{ nullptr };

		Delegate(void* callee, decltype(callerFunction) cF) : callerFunction(cF), callee(callee)
		{
		}
		
		template <class T, RET(T::* METHOD)(ARGS ...)>
		static RET methodCaller(void* callee, ARGS&&... params) { return (static_cast<T*>(callee)->*METHOD)(GTSL::ForwardRef<ARGS>(params)...); }

		template <class T, RET(T::* CONST_METHOD)(ARGS ...) const>
		static RET constMethodCaller(void* callee, ARGS&&... params) { return (static_cast<const T*>(callee)->*CONST_METHOD)(GTSL::ForwardRef<ARGS>(params)...); }

		template <RET(*FUNCTION)(ARGS ...)>
		static RET functionCaller(void* callee, ARGS&&... params) { return (FUNCTION)(GTSL::ForwardRef<ARGS>(params)...); }

		template <typename LAMBDA>
		static RET lambdaCaller(void* callee, ARGS&&... params) { return (static_cast<LAMBDA*>(callee)->operator())(GTSL::ForwardRef<ARGS>(params)...); }
	};
}