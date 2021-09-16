#pragma once

#include "Application.h"
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
		
		Result<GTSL::StaticString<128>> DoQuery(const Range<const char8_t*> query) {
			WIN32_FIND_DATAA find_data;

			if (!handle) {
				const auto handle_res = FindFirstFileA(reinterpret_cast<const char*>(query.begin()), &find_data);

				if (reinterpret_cast<uint64>(handle_res) != ERROR_FILE_NOT_FOUND && handle_res != INVALID_HANDLE_VALUE) {
					handle = handle_res;
					return Result<GTSL::StaticString<128>>(reinterpret_cast<const char8_t*>(find_data.cFileName));
				}

				return Result<GTSL::StaticString<128>>(false);
			} else {
				const auto handle_res = FindNextFileA(handle, &find_data);

				if (handle_res) {
					return Result<GTSL::StaticString<128>>(reinterpret_cast<const char8_t*>(find_data.cFileName));
				}

				FindClose(handle);

				return Result<GTSL::StaticString<128>>(false);
			}
		}
	
	private:
		handle_type handle = nullptr;
	};
}
