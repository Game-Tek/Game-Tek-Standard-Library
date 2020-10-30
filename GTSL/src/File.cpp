#include "GTSL/File.h"

#include <Windows.h>

#include "GTSL/Buffer.h"
#include "GTSL/StaticString.hpp"

using namespace GTSL;

File::~File()
{
	//GTSL_ASSERT(fileHandle == nullptr, "File was not closed!")
	if (fileHandle) { CloseHandle(static_cast<HANDLE>(fileHandle)); }
}

void File::OpenFile(const Range<const UTF8*> path, const AccessMode::value_type accessMode, const OpenMode openMode)
{
	DWORD access_mode{ 0 };

	if (static_cast<uint8>(accessMode) & static_cast<uint8>(AccessMode::READ)) { access_mode |= GENERIC_READ; }
	if (static_cast<uint8>(accessMode) & static_cast<uint8>(AccessMode::WRITE)) { access_mode |= GENERIC_WRITE; }

	DWORD open_mode{ 0 };

	switch (openMode)
	{
		case OpenMode::LEAVE_CONTENTS: open_mode = OPEN_ALWAYS; break;
		case OpenMode::CLEAR: open_mode = CREATE_ALWAYS; break;
		default: GTSL_ASSERT(false, "");
	}

	StaticString<MAX_PATH> win32_path(path);
	win32_path += '\0';
	fileHandle = CreateFileA(win32_path.begin(), access_mode, 0, nullptr, open_mode, FILE_ATTRIBUTE_NORMAL, nullptr);
	const auto w = GetLastError();
	GTSL_ASSERT(w == ERROR_SUCCESS || w == ERROR_ALREADY_EXISTS, "Win32 Error!");
}

void File::CloseFile()
{
	CloseHandle(static_cast<HANDLE>(fileHandle));
	//GTSL_ASSERT(GetLastError() == ERROR_SUCCESS, "Win32 Error!");
	fileHandle = nullptr;
}

uint32 File::WriteToFile(const Range<const byte*> buffer) const
{
	DWORD bytes{ 0 };
	WriteFile(static_cast<HANDLE>(fileHandle), buffer.begin(), static_cast<uint32>(buffer.Bytes()), &bytes, nullptr);
	//GTSL_ASSERT(GetLastError() == ERROR_SUCCESS, "Win32 Error!");
	return bytes;
}

uint32 File::WriteToFile(Buffer& buffer) const
{
	DWORD bytes{ 0 };
	WriteFile(static_cast<HANDLE>(fileHandle), buffer.GetData(), static_cast<uint32>(buffer.GetLength()), &bytes, nullptr);
	//GTSL_ASSERT(GetLastError() == ERROR_SUCCESS, "Win32 Error!");
	buffer.Resize(buffer.GetLength() - bytes);
	return bytes;
}

uint32 File::ReadFromFile(Range<byte*> buffer) const
{
	DWORD bytes{ 0 };
	[[maybe_unused]] auto res = ::ReadFile(static_cast<HANDLE>(fileHandle), buffer.begin(), static_cast<uint32>(buffer.Bytes()), &bytes, nullptr);
	const auto error = GetLastError();
	GTSL_ASSERT(error == ERROR_SUCCESS || res != 0, "Win32 Error!");
	return bytes;
}

uint32 File::ReadFile(Buffer& buffer) const
{
	DWORD bytes{ 0 };
	::ReadFile(static_cast<HANDLE>(fileHandle), buffer.GetData(), static_cast<uint32>(GetFileSize()), &bytes, nullptr);
	auto w = GetLastError();
	//GTSL_ASSERT(w , "Win32 Error!");
	buffer.Resize(bytes);
	return bytes;
}

void File::SetEndOfFile()
{
	::SetEndOfFile(fileHandle);
}

void File::SetPointer(const int64 byte, MoveFrom from)
{
	const LARGE_INTEGER bytes{ .QuadPart = byte };
	SetFilePointerEx(static_cast<HANDLE>(fileHandle), bytes, nullptr, static_cast<uint8>(from));
}

uint64 File::GetFileSize() const
{
	LARGE_INTEGER size;
	GetFileSizeEx(fileHandle, &size);
	//GTSL_ASSERT(GetLastError() == ERROR_SUCCESS, "Win32 Error!");
	return size.QuadPart;
}
