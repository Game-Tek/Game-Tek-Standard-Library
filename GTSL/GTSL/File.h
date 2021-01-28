#pragma once

#include "Core.h"
#include "Flags.h"

#include "Range.h"

namespace GTSL
{
	class BufferInterface;

	class File
	{		
	public:
		File() = default;
		~File();

		struct AccessMode : Flags<uint8> { static constexpr value_type READ = 1, WRITE = 2; };

		enum class OpenMode : uint8 { LEAVE_CONTENTS, CLEAR };
		
		void OpenFile(Range<const UTF8*> path, AccessMode::value_type accessMode, OpenMode openMode = OpenMode::LEAVE_CONTENTS);

		[[nodiscard]] uint32 WriteToFile(const Range<const byte*> buffer) const;
		[[nodiscard]] uint32 ReadFromFile(const Range<byte*> buffer) const;
		uint32 WriteToFile(BufferInterface buffer) const;
		
		uint32 ReadFile(BufferInterface buffer) const;
		uint32 ReadFile(uint64 size, BufferInterface buffer) const;
		uint32 ReadFile(GTSL::Range<byte*> buffer) const;
		uint32 ReadFile(uint64 size, uint64 offset, GTSL::Range<byte*> buffer) const;

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
