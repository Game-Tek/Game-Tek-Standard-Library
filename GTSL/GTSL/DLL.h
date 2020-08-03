#pragma once

#include "Core.h"
#include "Ranger.h"

namespace GTSL
{
	class DLL
	{
		#if (_WIN64)
		void* handle{ nullptr };
		#endif

	public:
		class DynamicFunction
		{
			#if (_WIN64)
			long long(__stdcall* functionPointer)() { nullptr };
			#endif
		public:
			using function_signature = decltype(functionPointer);
			
			DynamicFunction(function_signature function) : functionPointer(function)
			{
			}

			template<typename RET, typename... ARGS>
			RET operator()(ARGS&&... args) { return reinterpret_cast<RET(*)(ARGS...)>(functionPointer)(GTSL::ForwardRef<ARGS>(args)...); }

			template<typename RET, typename... ARGS>
			auto Call(ARGS&&... args) -> RET { return reinterpret_cast<RET(*)(ARGS...)>(functionPointer)(GTSL::ForwardRef<ARGS>(args)...); }
		};

		DLL() = default;
		~DLL() = default;
		
		void LoadLibrary(const Ranger<char>& ranger);
		void UnloadLibrary();
		
		[[nodiscard]] DynamicFunction LoadDynamicFunction(const Ranger<char>& ranger) const;
	};
}