//module;
#pragma once

#include <GTSL/Core.h>
#include "GTSL/StringCommon.h"

#include "File.hpp"
#include "Math/Math.hpp"

#if (_WIN64)
#include <Windows.h>
#include <memoryapi.h>
#endif

namespace GTSL {
	class MappedFile {
	public:
		MappedFile() = default;

		bool Open(const StringView path, uint64 fileSize, File::AccessMode access_mode) {
#if (_WIN64)
			DWORD shareMode = 0; /*exlusive use*/
			DWORD desiredAccess = GENERIC_READ; /*can be or'd*/
			DWORD creationDisposition = OPEN_ALWAYS;
			ULONG flProtect = PAGE_READONLY;
			ULONG mapViewOfFileDesiredAccess = FILE_MAP_READ;			
			
			if(access_mode & File::WRITE) {
				flProtect = PAGE_READWRITE;
				desiredAccess |= GENERIC_WRITE;
				mapViewOfFileDesiredAccess |= FILE_MAP_WRITE;
			} else {
			}
			
			file = CreateFileA(reinterpret_cast<const char*>(path.GetData()), desiredAccess, shareMode, nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
			auto createRes = GetLastError();
			if (!file) { return false; }

			if(access_mode & File::WRITE) {
				size = fileSize;				
			} else {
				GetFileSizeEx(file, reinterpret_cast<PLARGE_INTEGER>(&size));
			}
			
			fileMapping = CreateFileMappingA(file, nullptr, flProtect, 0, size, nullptr);
			auto fileMapRes = GetLastError();
			if (!fileMapping) { return false; }

			data = MapViewOfFile(fileMapping, mapViewOfFileDesiredAccess, 0, 0, size);
			auto mapViewRes = GetLastError();
			if (!data) { return false; }			

			return true;
#elif __linux__
			return false;
#endif
		}

		byte* GetData() {
#if (_WIN64)
			return static_cast<byte*>(data);
#elif __linux__
			return nullptr;
#endif
		}

		void Resize(const uint64 newSize) {
#if (_WIN64)
			size = newSize;
#elif __linux__
#endif
		}
		
		~MappedFile() {
#if (_WIN64)
			if (data)
				UnmapViewOfFile(data);

			if (fileMapping)
				CloseHandle(fileMapping);

			if (file)
				CloseHandle(file);
#elif __linux__
#endif
		}

	private:
#if (_WIN64)
		HANDLE file = nullptr, fileMapping = nullptr;
		void* data = nullptr; uint64 size = 0;
#elif __linux__
#endif
	};
}