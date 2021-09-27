#pragma once

#include "Application.h"
#include "StringCommon.h"
#include "String.hpp"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#define NO_COMM
#include <Windows.h>
#endif

namespace GTSL
{
	class FileQuery
	{
	public:
		using handle_type = void*;
		
		FileQuery() {}
		
		Result<StaticString<256>> DoQuery(const Range<const char8_t*> query) {
			WIN32_FIND_DATAA find_data;

			if (!handle) {
				const auto handle_res = FindFirstFileA(reinterpret_cast<const char*>(query.GetData()), &find_data);

				if (reinterpret_cast<uint64>(handle_res) != ERROR_FILE_NOT_FOUND && handle_res != INVALID_HANDLE_VALUE) {
					handle = handle_res;
					return Result(StaticString<256>(reinterpret_cast<const char8_t*>(find_data.cFileName)), true);
				}

				return Result<StaticString<256>>(false);
			} else {
				const auto handle_res = FindNextFileA(handle, &find_data);

				if (handle_res) {
					return Result(StaticString<256>(reinterpret_cast<const char8_t*>(find_data.cFileName)), true);
				}

				FindClose(handle);

				return Result<StaticString<256>>(false);
			}
		}
	
	private:
		handle_type handle = nullptr;
	};
}
