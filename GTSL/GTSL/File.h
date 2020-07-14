#pragma once

#include "Core.h"

#include "Ranger.h"

namespace GTSL
{
	class Buffer;

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

		uint32 WriteToFile(const Ranger<const byte>& buffer);
		uint32 ReadFromFile(const Ranger<byte>& buffer);
		uint32 WriteToFile(Buffer& buffer);
		uint32 ReadFile(Buffer& buffer);
		
		enum class MoveFrom : uint8
		{
			BEGIN, CURRENT, END
		};
		void SetPointer(uint64 byte, uint64& newFilePointer, MoveFrom from);

		[[nodiscard]] uint64 GetFileSize() const;
		
	private:
		void* fileHandle{ nullptr };
	};
}
