#include "GTSL/File.h"

#include <Windows.h>

#include "GTSL/Buffer.hpp"
#include "GTSL/StaticString.hpp"

using namespace GTSL;

File::~File()
{
	//GTSL_ASSERT(fileHandle == nullptr, "File was not closed!")
	if (fileHandle) { CloseHandle(static_cast<HANDLE>(fileHandle)); fileHandle = nullptr; }
}

void File::OpenFile(const Range<const UTF8*> path, const AccessMode::value_type accessMode, const OpenMode openMode)
{
	DWORD desiredAccess = 0;  DWORD shareMode = 0;

	if (static_cast<uint8>(accessMode) & static_cast<uint8>(AccessMode::READ)) { desiredAccess |= GENERIC_READ; shareMode |= FILE_SHARE_READ; }
	if (static_cast<uint8>(accessMode) & static_cast<uint8>(AccessMode::WRITE)) { desiredAccess |= GENERIC_WRITE; }

	DWORD creationDisposition = 0;

	switch (openMode)
	{
		case OpenMode::LEAVE_CONTENTS: creationDisposition = OPEN_ALWAYS; break;
		case OpenMode::CLEAR: creationDisposition = CREATE_ALWAYS; break;
		default: GTSL_ASSERT(false, "");
	}

	fileHandle = CreateFileA(path.begin(), desiredAccess, shareMode, nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
	const auto	w = GetLastError();
	GTSL_ASSERT(w == ERROR_SUCCESS || w == ERROR_ALREADY_EXISTS, "Win32 Error!");
}

uint32 File::WriteToFile(const Range<const byte*> buffer) const
{
	DWORD bytes{ 0 };
	WriteFile(static_cast<HANDLE>(fileHandle), buffer.begin(), static_cast<uint32>(buffer.Bytes()), &bytes, nullptr);
	//GTSL_ASSERT(GetLastError() == ERROR_SUCCESS, "Win32 Error!");
	return bytes;
}

uint32 File::WriteToFile(BufferInterface buffer) const
{
	DWORD bytes{ 0 };
	WriteFile(static_cast<HANDLE>(fileHandle), buffer.begin(), static_cast<uint32>(buffer.GetLength()), &bytes, nullptr);
	//GTSL_ASSERT(GetLastError() == ERROR_SUCCESS, "Win32 Error!");
	buffer.AddResize(-(int64)bytes);
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

uint32 File::ReadFile(BufferInterface buffer) const
{
	DWORD bytes{ 0 };
	::ReadFile(static_cast<HANDLE>(fileHandle), buffer.begin() + buffer.GetLength(), static_cast<uint32>(GetFileSize()), &bytes, nullptr);
	auto w = GetLastError();
	//GTSL_ASSERT(w , "Win32 Error!");
	buffer.AddResize(bytes);
	return bytes;
}

uint32 File::ReadFile(uint64 size, BufferInterface buffer) const
{
	DWORD bytes{ 0 };
	::ReadFile(static_cast<HANDLE>(fileHandle), buffer.begin() + buffer.GetLength(), static_cast<uint32>(size), &bytes, nullptr);
	auto w = GetLastError();
	//GTSL_ASSERT(w , "Win32 Error!");
	buffer.AddResize(bytes);
	return bytes;
}

uint32 File::ReadFile(GTSL::Range<byte*> buffer) const
{
	DWORD bytes{ 0 };
	::ReadFile(static_cast<HANDLE>(fileHandle), buffer.begin(), static_cast<uint32>(buffer.Bytes()), &bytes, nullptr);
	auto w = GetLastError();
	//GTSL_ASSERT(w , "Win32 Error!");
	return bytes;
}

uint32 File::ReadFile(uint64 size, uint64 offset, GTSL::Range<byte*> buffer) const
{
	DWORD bytes{ 0 };
	::ReadFile(static_cast<HANDLE>(fileHandle), buffer.begin() + offset, static_cast<uint32>(size), &bytes, nullptr);
	auto w = GetLastError();
	GTSL_ASSERT(buffer.begin() + offset + size < buffer.end() , "Trying to write outside of buffer!");
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
