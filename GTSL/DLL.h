#pragma once

#include "Core.h"
#include "FunctionPointer.hpp"
#include "StringCommon.h"

#if (_WIN64)
#include <Windows.h>
#undef LoadLibrary
#endif

namespace GTSL
{
	class DLL {
#if _WIN64
		HMODULE handle{ nullptr };
#elif __linux__
		int handle = 0;
#endif

	public:
		class DynamicFunction
		{
#if (_WIN64)
			long long(__stdcall* functionPointer)() { nullptr };
#elif __linux__
			void(*functionPointer)() { nullptr };
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
		
		bool LoadLibrary(const StringView ranger) {
#if (_WIN64)
			handle = LoadLibraryA(reinterpret_cast<const char*>(ranger.GetData()));
			return handle;
#elif __linux__
			return false;
#endif
		}
		
		void TryUnload() {
			if (handle)
				UnloadLibrary();
		}
		void UnloadLibrary() {
#if _WIN64
			FreeLibrary(handle);
			handle = nullptr;
#elif __linux__
			handle = 0;
#endif
		}

		template<typename T>
		void LoadDynamicFunction(const char8_t* name, T* func) const {
#if _WIN64
			*func = reinterpret_cast<T>(GetProcAddress(handle, reinterpret_cast<const char*>(name)));
#elif __linux__
#endif
		}

		template<typename RET, typename... PARAMS>
		[[nodiscard]] FunctionPointer<RET(PARAMS...)> LoadDynamicFunction(const char8_t* name) const {
#if _WIN64
			return FunctionPointer<RET(PARAMS...)>(reinterpret_cast<RET(__stdcall *)(PARAMS...)>(GetProcAddress(handle, reinterpret_cast<const char*>(name))));
#elif __linux__
#endif
		}

		template<typename RET, typename... PARAMS>
		[[nodiscard]] auto LoadDynamicFunction(const Range<const char8_t*> ranger) const {
#if _WIN64
			return FunctionPointer<RET(PARAMS...)>(reinterpret_cast<RET(__stdcall *)(PARAMS...)>(GetProcAddress(handle, reinterpret_cast<const char*>(ranger.begin()))));
#elif __linux__
#endif
		}
	};
}