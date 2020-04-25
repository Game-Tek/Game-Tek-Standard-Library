#include "GTSL/File.h"

#include <Windows.h>

using namespace GTSL;


void File::OpenFile(const Ranger<char>& path, OpenFileMode openFileMode)
{
	uint64 open_mode{ 0 };

	if (static_cast<uint8>(openFileMode) & static_cast<uint8>(OpenFileMode::READ))
	{
		open_mode = open_mode | GENERIC_READ;
	}
	if (static_cast<uint8>(openFileMode) & static_cast<uint8>(OpenFileMode::WRITE))
	{
		open_mode = open_mode | GENERIC_WRITE;
	}

	fileHandle = CreateFileA(path.begin(), open_mode, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
}

void File::CloseFile()
{
	CloseHandle(static_cast<HANDLE>(fileHandle));
}

void File::WriteToFile(const Ranger<byte>& buffer, uint64& bytesWritten)
{
	DWORD bytes{ 0 };
	WriteFile(static_cast<HANDLE>(fileHandle), buffer.begin(), buffer.Bytes(), &bytes, nullptr);
	bytesWritten = bytes;
}

void File::ReadFromFile(const Ranger<byte>& buffer, uint64& bytesRead)
{
	DWORD bytes{ 0 };
	ReadFile(static_cast<HANDLE>(fileHandle), const_cast<byte*>(buffer.begin()), buffer.Bytes(), &bytes, nullptr);
	bytesRead = bytes;
}

void File::SetPointer(const uint64 byte, uint64& newFilePointer, MoveFrom from)
{
	LARGE_INTEGER bytes{ byte };
	LARGE_INTEGER bytes_moved{ 0 };
	SetFilePointerEx(static_cast<HANDLE>(fileHandle), bytes, &bytes_moved, static_cast<uint8>(from));
	newFilePointer = bytes_moved.QuadPart;
}
