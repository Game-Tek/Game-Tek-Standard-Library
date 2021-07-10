#pragma once

#include "Core.h"
#include "FunctionPointer.hpp"
#include "Range.h"

#if (_WIN64)
#include <Windows.h>
#undef LoadLibrary
#endif

namespace GTSL
{
	class DLL
	{
		HMODULE handle{ nullptr };

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
		~DLL() noexcept {
			TryUnload();
		}
		
		bool LoadLibrary(const char8_t* name) { handle = LoadLibraryA(reinterpret_cast<const char*>(name)); return handle; }
		
		bool LoadLibrary(const Range<const char8_t*> ranger) {
			handle = LoadLibraryA(reinterpret_cast<const char*>(ranger.begin()));
			return handle;
		}
		
		void TryUnload() {
			if (handle)
				UnloadLibrary();
		}
		void UnloadLibrary() {
			FreeLibrary(handle);
			handle = nullptr;
		}

		template<typename T>
		void LoadDynamicFunction(const char8_t* name, T* func) const {
			*func = reinterpret_cast<T>(GetProcAddress(handle, reinterpret_cast<const char*>(name)));
		}

		//[[nodiscard]] DynamicFunction LoadDynamicFunction(const Range<const char*> ranger) const {
		//	return DynamicFunction(GetProcAddress(handle, ranger.begin()));
		//}

		template<typename RET, typename... PARAMS>
		[[nodiscard]] FunctionPointer<RET(PARAMS...)> LoadDynamicFunction(const char8_t* name) const {
			return FunctionPointer<RET(PARAMS...)>(reinterpret_cast<RET(__stdcall *)(PARAMS...)>(GetProcAddress(handle, reinterpret_cast<const char*>(name))));
		}

		template<typename RET, typename... PARAMS>
		[[nodiscard]] auto LoadDynamicFunction(const Range<const char8_t*> ranger) const {
			return FunctionPointer<RET(PARAMS...)>(reinterpret_cast<RET(__stdcall *)(PARAMS...)>(GetProcAddress(handle, reinterpret_cast<const char*>(ranger.begin()))));
		}
	};
}