#pragma once

#include "Core.h"

#include "Ranger.h"

namespace GTSL
{
	class File
	{		
	public:
		File() = default;
		~File();

		enum class OpenFileMode : uint8
		{
			READ = 1, WRITE = 2
		};
		void OpenFile(const Ranger<const UTF8>& path, OpenFileMode openFileMode);
		void CloseFile();

		void WriteToFile(Ranger<const byte>& buffer);
		void ReadFromFile(Ranger<byte>& buffer);
		
		enum class MoveFrom : uint8
		{
			BEGIN, CURRENT, END
		};
		void SetPointer(uint64 byte, uint64& newFilePointer, MoveFrom from);

	private:
		void* fileHandle{ nullptr };
	};
}
