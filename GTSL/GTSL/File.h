#pragma once

#include "Core.h"

#include "Ranger.h"

namespace GTSL
{
	class File
	{
		void* fileHandle{ nullptr };
	public:

		File() = default;

		enum class OpenFileMode : uint8
		{
			READ = 1,
			WRITE = 2
		};
		void OpenFile(const Ranger<char>& path, OpenFileMode openFileMode);
		void CloseFile();

		void WriteToFile(const Ranger<byte>& buffer, uint64& bytesWritten);
		void ReadFromFile(const Ranger<byte>& buffer, uint64& bytesRead);
		
		enum class MoveFrom : uint8
		{
			BEGIN, CURRENT, END
		};
		void SetPointer(uint64 byte, uint64& newFilePointer, MoveFrom from);
		
		~File() = default;
	};
}
