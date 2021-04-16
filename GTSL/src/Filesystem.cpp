#include "GTSL/Filesystem.h"

#include <Windows.h>

bool GTSL::FileQuery::DoQuery()
{
	WIN32_FIND_DATAA find_data;

	if (!handle)
	{
		const auto handle_res = FindFirstFileA(query.begin(), &find_data);

		if (reinterpret_cast<uint64>(handle_res) != ERROR_FILE_NOT_FOUND && handle_res != INVALID_HANDLE_VALUE)
		{
			fileNameWithExtension = find_data.cFileName; handle = handle_res;
			return true;
		}
		
		return false;
	}
	else
	{
		const auto handle_res = FindNextFileA(handle, &find_data);

		if (handle_res) {
			fileNameWithExtension = find_data.cFileName;
			return true;
		}

		FindClose(handle);
		
		return false;
	}
}