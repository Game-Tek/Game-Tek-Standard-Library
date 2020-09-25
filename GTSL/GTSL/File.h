#pragma once

#include "Core.h"

#include "Range.h"

namespace GTSL
{
	class Buffer;

	class File
	{		
	public:
		File() = default;
		~File();

		enum class AccessMode : uint8 { READ = 1, WRITE = 2 };

		enum class OpenMode : uint8 { LEAVE_CONTENTS, CLEAR };
		void OpenFile(const Range<const UTF8*> path, uint8 accessMode, OpenMode openMode);
		void CloseFile();

		[[nodiscard]] uint32 WriteToFile(const Range<const byte*> buffer) const;
		[[nodiscard]] uint32 ReadFromFile(const Range<byte*> buffer) const;
		uint32 WriteToFile(Buffer& buffer) const;
		uint32 ReadFile(Buffer& buffer) const;

		void SetEndOfFile();
		
		enum class MoveFrom : uint8
		{
			BEGIN, CURRENT, END
		};
		void SetPointer(int64 byte, MoveFrom from);

		[[nodiscard]] uint64 GetFileSize() const;
		
	private:
		void* fileHandle{ nullptr };
	};
}
