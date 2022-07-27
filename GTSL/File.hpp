#pragma once

#include "Buffer.hpp"
#include "Core.h"
#include "Flags.h"

#include "Range.hpp"
#include "StringCommon.h"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#define NO_COMM
#define NO_MIN_MAX
#include <Windows.h>
#undef ERROR
#endif

namespace GTSL {
	template<typename T>
	struct OptionalParameter {
		OptionalParameter() = default;
		OptionalParameter(const T value) : isSet(true), value(value) {}

		explicit operator bool() const { return isSet; }

		T operator()(const T alternative) { return isSet ? value : alternative; }

		operator T() { return value; }
	private:
		bool isSet = false;
		T value;
	};

	class File {
	public:
		File() = default;

		using AccessMode = Flags<uint8, struct AccessModeFlag>;
		static constexpr AccessMode READ{ 1 }, WRITE{ 2 };

		enum class OpenMode : uint8 { LEAVE_CONTENTS, CLEAR };

		enum class OpenResult { OK, CREATED, ERROR };

		File(StringView path, AccessMode accessMode = READ, bool create = false) {
			auto result = Open(path, accessMode, create);
		}

		~File() {
#if (_WIN64)
			if (fileHandle) { CloseHandle(fileHandle); fileHandle = nullptr; }
#elif (__linux__)
			if (file)
#endif
		}
		
		[[nodiscard]] OpenResult Open(StringView path, AccessMode accessMode = READ, bool create = false) {
			DWORD desiredAccess = 0;  DWORD shareMode = 0;

			if (static_cast<uint8>(accessMode) & static_cast<uint8>(READ)) { desiredAccess |= GENERIC_READ; shareMode |= FILE_SHARE_READ; }
			if (static_cast<uint8>(accessMode) & static_cast<uint8>(WRITE)) { desiredAccess |= GENERIC_WRITE; shareMode |= FILE_SHARE_WRITE; }

			DWORD creationDisposition = create ? OPEN_ALWAYS : OPEN_EXISTING;

			fileHandle = CreateFileA(reinterpret_cast<const char*>(path.GetData()), desiredAccess, shareMode, nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);

			OpenResult openResult;

			switch (GetLastError()) {
			case ERROR_SUCCESS: openResult = OpenResult::OK; break;
			case ERROR_ALREADY_EXISTS: openResult = OpenResult::CREATED; break;
			default: openResult = OpenResult::ERROR; break;
			}

			return openResult;
		}

		uint32 Write(const uint64 size, const byte* d) const {
			DWORD bytes{ 0 };
			WriteFile(fileHandle, d, static_cast<uint32>(size), &bytes, nullptr);
			return bytes;
		}

		uint32 Write(const Range<const byte*> buffer) const {
			DWORD bytes{ 0 };
			WriteFile(fileHandle, buffer.begin(), static_cast<uint32>(buffer.Bytes()), &bytes, nullptr);
			return bytes;
		}

		template<class B>
		uint32 Write(B& buffer, OptionalParameter<uint64> offset = {0}, OptionalParameter<uint64> size = {}) const {
			DWORD bytes{ 0 };
			WriteFile(fileHandle, buffer.begin(), static_cast<uint32>(size(buffer.GetLength())), &bytes, nullptr);
			buffer.modifyOccupiedBytes(-static_cast<int64>(size(buffer.GetLength())));
			return bytes;
		}
		
		template<class B>
		uint32 Read(B& buffer, OptionalParameter<uint64> size = {}) const {
			DWORD bytes{ 0 };
			buffer.DeltaResize(size(GetSize()));
			ReadFile(fileHandle, buffer.begin() + buffer.GetLength(), static_cast<uint32>(size(GetSize())), &bytes, nullptr);
			buffer.modifyOccupiedBytes(bytes);
			return bytes;
		}

		void Resize(const uint64 newSize) {
			GTSL_ASSERT(newSize < (~(0ULL)) / 2, "Number too large.");
			const LARGE_INTEGER bytes{ .QuadPart = static_cast<LONGLONG>(newSize) };
			SetFilePointerEx(fileHandle, bytes, nullptr, FILE_BEGIN);
			SetEndOfFile(fileHandle);
		}
		
		void SetPointer(uint64 byte) {
			GTSL_ASSERT(byte < (~(0ULL)) / 2, "Number too large.");
			const LARGE_INTEGER bytes{ .QuadPart = static_cast<LONGLONG>(byte) };
			SetFilePointerEx(fileHandle, bytes, nullptr, FILE_BEGIN);
		}

		[[nodiscard]] uint64 GetSize() const {
			LARGE_INTEGER size;
			auto res = GetFileSizeEx(fileHandle, &size);
			GTSL_ASSERT(res != 0, "Win32 Error!");
			return size.QuadPart;
		}

		uint64 GetFileHash() const {
			FILETIME filetime;
			GetFileTime(fileHandle, nullptr, nullptr, &filetime);
			return filetime.dwLowDateTime | static_cast<uint64>(filetime.dwHighDateTime) << 32ull;
		}

		explicit operator bool() const { return fileHandle; }
	
	private:
		void* fileHandle{ nullptr };
	};
}
