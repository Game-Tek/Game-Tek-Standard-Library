#include "GTSL/File.h"

#include <Windows.h>

#include "GTSL/StaticString.hpp"

using namespace GTSL;

File::~File()
{
	GTSL_ASSERT(fileHandle == nullptr, "File was not closed!")
}

void File::OpenFile(const Ranger<const UTF8>& path, OpenFileMode openFileMode)
{	
	uint64 open_mode{ 0 };

	if (static_cast<uint8>(openFileMode) & static_cast<uint8>(OpenFileMode::READ)) { open_mode |= GENERIC_READ; }
	if (static_cast<uint8>(openFileMode) & static_cast<uint8>(OpenFileMode::WRITE)) { open_mode |= GENERIC_WRITE; }

	StaticString<MAX_PATH> win32_path(path);
	win32_path += '\0';
	fileHandle = CreateFileA(win32_path.begin(), open_mode, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
}

void File::CloseFile()
{
	CloseHandle(static_cast<HANDLE>(fileHandle));
#if (_DEBUG)
	fileHandle = nullptr;
#endif
}

void File::WriteToFile(Ranger<const byte>& buffer)
{
	DWORD bytes{ 0 };
	WriteFile(static_cast<HANDLE>(fileHandle), buffer.begin(), buffer.Bytes(), &bytes, nullptr);
	buffer = Ranger<const byte>(bytes, buffer.begin());
}

void File::ReadFromFile(Ranger<byte>& buffer)
{
	DWORD bytes{ 0 };
	ReadFile(static_cast<HANDLE>(fileHandle), buffer.begin(), buffer.Bytes(), &bytes, nullptr);
	buffer = Ranger<byte>(bytes, buffer.begin());
}

void File::SetPointer(const uint64 byte, uint64& newFilePointer, MoveFrom from)
{
	LARGE_INTEGER bytes{ byte }; LARGE_INTEGER bytes_moved{ 0 };
	SetFilePointerEx(static_cast<HANDLE>(fileHandle), bytes, &bytes_moved, static_cast<uint8>(from));
	newFilePointer = bytes_moved.QuadPart;
}
