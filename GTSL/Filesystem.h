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
		
		FileQuery(const StaticString<Application::MAX_PATH_LENGTH>& query) : query(query)
		{
		}
		
		bool DoQuery() {
			WIN32_FIND_DATAA find_data;

			if (!handle) {
				const auto handle_res = FindFirstFileA(reinterpret_cast<const char*>(query.begin()), &find_data);

				if (reinterpret_cast<uint64>(handle_res) != ERROR_FILE_NOT_FOUND && handle_res != INVALID_HANDLE_VALUE)
				{
					fileNameWithExtension = reinterpret_cast<const char8_t*>(find_data.cFileName); handle = handle_res;
					return true;
				}

				return false;
			} else {
				const auto handle_res = FindNextFileA(handle, &find_data);

				if (handle_res) {
					fileNameWithExtension = reinterpret_cast<const char8_t*>(find_data.cFileName);
					return true;
				}

				FindClose(handle);

				return false;
			}
		}

		operator bool() { return DoQuery(); }

		auto GetFileNameWithExtension() const { return fileNameWithExtension; }
	
	private:
		StaticString<Application::MAX_PATH_LENGTH> query;
		StaticString<Application::MAX_PATH_LENGTH> fileNameWithExtension;
		handle_type handle = nullptr;
	};
}
