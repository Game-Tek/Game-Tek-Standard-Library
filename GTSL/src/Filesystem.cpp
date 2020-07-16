#include "GTSL/Filesystem.h"

#include <Windows.h>

bool GTSL::FileQuery::StartQuery(QueryResult& queryResult)
{
	WIN32_FIND_DATAA find_data;
	const auto handle_res = FindFirstFileA(query.begin(), &find_data);

	if(reinterpret_cast<uint64>(handle_res) != ERROR_FILE_NOT_FOUND && handle_res != INVALID_HANDLE_VALUE)
	{
		queryResult.FileNameWithExtension = find_data.cFileName; handle = handle_res;
		return true;
	}

	return false;
}

bool GTSL::FileQuery::NextQuery(QueryResult& queryResult) const
{
	WIN32_FIND_DATAA find_data;
	const auto handle_res = FindNextFileA(handle, &find_data);

	if (handle_res)
	{
		queryResult.FileNameWithExtension = find_data.cFileName;
		return true;
	}

	return false;
}

void GTSL::FileQuery::EndQuery() const { FindClose(handle); }
