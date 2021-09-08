//module;
#pragma once

#include <GTSL/Core.h>
#include <GTSL/Range.hpp>

#include "File.h"
#include "Math/Math.hpp"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <memoryapi.h>
#endif

namespace GTSL {
	class MappedFile {
	public:
		MappedFile() = default;

		bool Open(const Range<const char8_t*> path, uint64 fileSize, File::AccessMode access_mode) {

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
			
			file = CreateFileA(reinterpret_cast<const char*>(path.begin()), desiredAccess, shareMode, nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
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
		}

		byte* GetData() {
			return static_cast<byte*>(data);
		}

		void Resize(const uint64 newSize) {
			size = newSize;
		}
		
		~MappedFile() {
			if (data)
				UnmapViewOfFile(data);

			if (fileMapping)
				CloseHandle(fileMapping);

			if (file)
				CloseHandle(file);
		}

	private:
		HANDLE file, fileMapping;
		void* data; uint64 size = 0;
	};
}