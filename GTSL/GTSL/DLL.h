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
		
		bool LoadLibrary(const UTF8* name) { handle = LoadLibraryA(name); return handle; }
		
		bool LoadLibrary(const Range<const char*> ranger) {
			handle = LoadLibraryA(ranger.begin());
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
		void LoadDynamicFunction(const UTF8* name, T* func) const {
			*func = reinterpret_cast<T>(GetProcAddress(handle, name));
		}

		//[[nodiscard]] DynamicFunction LoadDynamicFunction(const Range<const char*> ranger) const {
		//	return DynamicFunction(GetProcAddress(handle, ranger.begin()));
		//}

		template<typename RET, typename... PARAMS>
		[[nodiscard]] FunctionPointer<RET(PARAMS...)> LoadDynamicFunction(const UTF8* name) const {
			return FunctionPointer<RET(PARAMS...)>(reinterpret_cast<RET(__stdcall *)(PARAMS...)>(GetProcAddress(handle, name)));
		}

		template<typename RET, typename... PARAMS>
		[[nodiscard]] auto LoadDynamicFunction(const Range<const char*> ranger) const {
			return FunctionPointer<RET(PARAMS...)>(reinterpret_cast<RET(__stdcall *)(PARAMS...)>(GetProcAddress(handle, ranger.begin())));
		}
	};
}