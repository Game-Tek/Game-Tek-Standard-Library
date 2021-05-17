#include "GTSL/File.h"

#include <Windows.h>

#include "GTSL/Buffer.hpp"
#include "GTSL/StaticString.hpp"

using namespace GTSL;
#undef ERROR

File::~File()
{
	//GTSL_ASSERT(fileHandle == nullptr, "File was not closed!")
	if (fileHandle) { CloseHandle(static_cast<HANDLE>(fileHandle)); fileHandle = nullptr; }
}

File::OpenResult File::Create(Range<const UTF8*> path, AccessMode accessMode)
{
	DWORD desiredAccess = 0;  DWORD shareMode = 0;

	if (static_cast<uint8>(accessMode) & static_cast<uint8>(READ)) { desiredAccess |= GENERIC_READ; shareMode |= FILE_SHARE_READ; }
	if (static_cast<uint8>(accessMode) & static_cast<uint8>(WRITE)) { desiredAccess |= GENERIC_WRITE; }

	DWORD creationDisposition = CREATE_NEW;

	fileHandle = CreateFileA(path.begin(), desiredAccess, shareMode, nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);

	File::OpenResult openResult;

	switch (GetLastError()) {
	case ERROR_SUCCESS: openResult = OpenResult::OK; break;
	case ERROR_FILE_EXISTS: openResult = OpenResult::ALREADY_EXISTS; break;
	case ERROR_FILE_NOT_FOUND: openResult = OpenResult::DOES_NOT_EXIST; break;
	default: openResult = OpenResult::ERROR; break;
	}

	return openResult;
}

File::OpenResult File::Open(const Range<const UTF8*> path, const AccessMode accessMode)
{
	DWORD desiredAccess = 0;  DWORD shareMode = 0;

	if (static_cast<uint8>(accessMode) & static_cast<uint8>(READ)) { desiredAccess |= GENERIC_READ; shareMode |= FILE_SHARE_READ; }
	if (static_cast<uint8>(accessMode) & static_cast<uint8>(WRITE)) { desiredAccess |= GENERIC_WRITE; }

	DWORD creationDisposition = OPEN_EXISTING;

	fileHandle = CreateFileA(path.begin(), desiredAccess, shareMode, nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);

	File::OpenResult openResult;

	switch (GetLastError()) {
	case ERROR_SUCCESS: openResult = OpenResult::OK; break;
	case ERROR_FILE_NOT_FOUND: openResult = OpenResult::DOES_NOT_EXIST; break;
	default: openResult = OpenResult::ERROR; break;
	}
	
	return openResult;
}

uint32 File::Write(const Range<const byte*> buffer) const
{
	DWORD bytes{ 0 };
	WriteFile(static_cast<HANDLE>(fileHandle), buffer.begin(), static_cast<uint32>(buffer.Bytes()), &bytes, nullptr);
	//GTSL_ASSERT(GetLastError() == ERROR_SUCCESS, "Win32 Error!");
	return bytes;
}

uint32 File::Write(BufferInterface buffer) const
{
	DWORD bytes{ 0 };
	WriteFile(static_cast<HANDLE>(fileHandle), buffer.begin(), static_cast<uint32>(buffer.GetLength()), &bytes, nullptr);
	//GTSL_ASSERT(GetLastError() == ERROR_SUCCESS, "Win32 Error!");
	buffer.AddResize(-(int64)bytes);
	return bytes;
}

uint32 File::Read(BufferInterface buffer) const
{
	DWORD bytes{ 0 };
	ReadFile(static_cast<HANDLE>(fileHandle), buffer.begin() + buffer.GetLength(), GetSize(), &bytes, nullptr);
	auto w = GetLastError();
	//GTSL_ASSERT(w , "Win32 Error!");
	buffer.AddResize(bytes);
	return bytes;
}

uint32 File::Read(uint64 size, BufferInterface buffer) const
{
	DWORD bytes{ 0 };
	ReadFile(static_cast<HANDLE>(fileHandle), buffer.begin() + buffer.GetLength(), static_cast<uint32>(size), &bytes, nullptr);
	auto w = GetLastError();
	//GTSL_ASSERT(w , "Win32 Error!");
	buffer.AddResize(bytes);
	return bytes;
}

uint32 File::Read(GTSL::Range<byte*> buffer) const
{
	DWORD bytes{ 0 };
	ReadFile(static_cast<HANDLE>(fileHandle), buffer.begin(), static_cast<uint32>(buffer.Bytes()), &bytes, nullptr);
	auto w = GetLastError();
	//GTSL_ASSERT(w , "Win32 Error!");
	return bytes;
}

uint32 File::Read(uint64 size, uint64 offset, GTSL::Range<byte*> buffer) const
{
	DWORD bytes{ 0 };
	ReadFile(static_cast<HANDLE>(fileHandle), buffer.begin() + offset, static_cast<uint32>(size), &bytes, nullptr);
	auto w = GetLastError();
	GTSL_ASSERT(buffer.begin() + offset + size < buffer.end() , "Trying to write outside of buffer!");
	return bytes;
}

void File::Resize(const uint64 newSize) {
	GTSL_ASSERT(newSize < (~(0ULL)) / 2, "Number too large.");
	const LARGE_INTEGER bytes{ .QuadPart = static_cast<LONGLONG>(newSize) };
	SetFilePointerEx(static_cast<HANDLE>(fileHandle), bytes, nullptr, FILE_BEGIN);
	SetEndOfFile(fileHandle);
}

void File::SetPointer(uint64 byte) {
	GTSL_ASSERT(byte < (~(0ULL)) / 2, "Number too large.");
	const LARGE_INTEGER bytes{ .QuadPart = static_cast<LONGLONG>(byte) };
	SetFilePointerEx(static_cast<HANDLE>(fileHandle), bytes, nullptr, FILE_BEGIN);
}

uint64 File::GetSize() const {
	LARGE_INTEGER size;
	auto res = GetFileSizeEx(fileHandle, &size);
	GTSL_ASSERT(res != 0, "Win32 Error!");
	return size.QuadPart;
}
