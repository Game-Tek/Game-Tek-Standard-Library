#pragma once

#include "Core.h"
#include "Flags.h"

#include "Range.h"

#undef ERROR

namespace GTSL
{
	class BufferInterface;

	class File
	{		
	public:
		File() = default;
		~File();

		using AccessMode = Flags<uint8, struct AccessModeFlag>;
		static constexpr AccessMode READ = 1, WRITE = 2;

		enum class OpenMode : uint8 { LEAVE_CONTENTS, CLEAR };

		enum class OpenResult { OK, ALREADY_EXISTS, DOES_NOT_EXIST, ERROR };
		
		[[nodiscard]] OpenResult Create(Range<const UTF8*> path, AccessMode accessMode);
		[[nodiscard]] OpenResult Open(Range<const UTF8*> path, AccessMode accessMode);

		uint32 Write(const Range<const byte*> buffer) const;
		uint32 Write(BufferInterface buffer) const;
		
		[[nodiscard]] uint32 Read(const Range<byte*> buffer) const;
		uint32 Read(BufferInterface buffer) const;
		uint32 Read(uint64 size, BufferInterface buffer) const;
		uint32 Read(uint64 size, uint64 offset, GTSL::Range<byte*> buffer) const;

		void Resize(const uint64 newSize);
		
		void SetPointer(uint64 byte);

		[[nodiscard]] uint64 GetSize() const;
		
	private:
		void* fileHandle{ nullptr };
	};
}
