#pragma once

#include "Buffer.hpp"
#include "Core.h"
#include "Flags.h"

#include "Range.hpp"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#define NO_COMM
#define NO_MIN_MAX
#include <Windows.h>
#undef ERROR
#endif

namespace GTSL {
	class File {
	public:
		File() = default;
		
		~File() {
#if (_WIN64)
			if (fileHandle) { CloseHandle(fileHandle); fileHandle = nullptr; }
#elif (__linux__)
			if (file)
#endif
		}

		using AccessMode = Flags<uint8, struct AccessModeFlag>;
		static constexpr AccessMode READ{ 1 }, WRITE{ 2 };

		enum class OpenMode : uint8 { LEAVE_CONTENTS, CLEAR };

		enum class OpenResult { OK, CREATED, ERROR };
		
		[[nodiscard]] OpenResult Open(Range<const char8_t*> path, AccessMode accessMode, bool create) {
			DWORD desiredAccess = 0;  DWORD shareMode = 0;

			if (static_cast<uint8>(accessMode) & static_cast<uint8>(READ)) { desiredAccess |= GENERIC_READ; shareMode |= FILE_SHARE_READ; }
			if (static_cast<uint8>(accessMode) & static_cast<uint8>(WRITE)) { desiredAccess |= GENERIC_WRITE; shareMode |= FILE_SHARE_WRITE; }

			DWORD creationDisposition = create ? OPEN_ALWAYS : OPEN_EXISTING;

			fileHandle = CreateFileA(reinterpret_cast<const char*>(path.begin()), desiredAccess, shareMode, nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);

			OpenResult openResult;

			switch (GetLastError()) {
			case ERROR_SUCCESS: openResult = OpenResult::CREATED; break;
			case ERROR_ALREADY_EXISTS: openResult = OpenResult::OK; break;
			default: openResult = OpenResult::ERROR; break;
			}

			return openResult;
		}

		uint32 Write(const Range<const byte*> buffer) const {
			DWORD bytes{ 0 };
			WriteFile(fileHandle, buffer.begin(), static_cast<uint32>(buffer.Bytes()), &bytes, nullptr);
			//GTSL_ASSERT(GetLastError() == ERROR_SUCCESS, "Win32 Error!");
			return bytes;
		}
		
		template<class B>
		uint32 Write(B& buffer) const {
			DWORD bytes{ 0 };
			WriteFile(fileHandle, buffer.begin(), static_cast<uint32>(buffer.GetLength()), &bytes, nullptr);
			buffer.AddBytes(-static_cast<int64>(bytes));
			return bytes;
		}
		
		[[nodiscard]] uint32 Read(const Range<byte*> buffer) const
		{
			DWORD bytes{ 0 };
			ReadFile(fileHandle, buffer.begin(), static_cast<uint32>(buffer.Bytes()), &bytes, nullptr);
			auto w = GetLastError();
			//GTSL_ASSERT(w , "Win32 Error!");
			return bytes;
		}

		template<class B>
		uint32 Read(B& buffer) const {
			DWORD bytes{ 0 };
			buffer.AddResize(GetSize());
			ReadFile(fileHandle, buffer.begin() + buffer.GetLength(), static_cast<DWORD>(GetSize()), &bytes, nullptr);
			buffer.AddBytes(GetSize());
			auto w = GetLastError();
			return bytes;
		}
		
		template<class B>
		uint32 Read(uint64 size, B& buffer) const {
			DWORD bytes{ 0 };
			buffer.AddResize(bytes);
			ReadFile(fileHandle, buffer.begin() + buffer.GetLength(), static_cast<uint32>(size), &bytes, nullptr);
			buffer.AddBytes(GetSize());
			auto w = GetLastError();
			return bytes;
		}
		
		uint32 Read(uint64 size, uint64 offset, GTSL::Range<byte*> buffer) const
		{
			DWORD bytes{ 0 };
			ReadFile(fileHandle, buffer.begin() + offset, static_cast<uint32>(size), &bytes, nullptr);
			auto w = GetLastError();
			GTSL_ASSERT(buffer.begin() + offset + size <= buffer.end(), "Trying to write outside of buffer!");
			return bytes;
		}

		void Resize(const uint64 newSize)
		{
			GTSL_ASSERT(newSize < (~(0ULL)) / 2, "Number too large.");
			const LARGE_INTEGER bytes{ .QuadPart = static_cast<LONGLONG>(newSize) };
			SetFilePointerEx(fileHandle, bytes, nullptr, FILE_BEGIN);
			SetEndOfFile(fileHandle);
		}
		
		void SetPointer(uint64 byte)
		{
			GTSL_ASSERT(byte < (~(0ULL)) / 2, "Number too large.");
			const LARGE_INTEGER bytes{ .QuadPart = static_cast<LONGLONG>(byte) };
			SetFilePointerEx(fileHandle, bytes, nullptr, FILE_BEGIN);
		}

		[[nodiscard]] uint64 GetSize() const
		{
			LARGE_INTEGER size;
			auto res = GetFileSizeEx(fileHandle, &size);
			GTSL_ASSERT(res != 0, "Win32 Error!");
			return size.QuadPart;
		}

		explicit operator bool() const { return fileHandle; }
	
	private:
		void* fileHandle{ nullptr };
	};
}
