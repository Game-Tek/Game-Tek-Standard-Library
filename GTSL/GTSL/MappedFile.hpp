//module;
#pragma once

#include <GTSL/Core.h>
#include <GTSL/Range.h>
#include <Windows.h>

//export module MappedFile;

namespace GTSL {
	class MappedFile {
	public:
		MappedFile() = default;

		bool Open(const Range<const char8_t*> path) {

			DWORD shareMode = 0; /*exlusive use*/
			DWORD desiredAccess = GENERIC_READ;
			DWORD creationDisposition = OPEN_EXISTING;
			file = CreateFileA(reinterpret_cast<const char*>(path.begin()), desiredAccess, shareMode, nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (!file) { return false; }

			LARGE_INTEGER large_integer;
			GetFileSizeEx(file, &large_integer);
			size = large_integer.QuadPart;
			
			fileMapping = CreateFileMappingA(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
			if (!fileMapping) { return false; }

			data = MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);
			if (!data) { return false; }

			return true;
		}

		bool Open(const Range<const char8_t*> path, uint64 fileSize) {

			DWORD shareMode = 0; /*exlusive use*/
			DWORD desiredAccess = GENERIC_READ | GENERIC_WRITE;
			DWORD creationDisposition = OPEN_EXISTING;
			file = CreateFileA(reinterpret_cast<const char*>(path.begin()), desiredAccess, shareMode, nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (!file) { return false; }

			size = fileSize;

			ULONG desiredAccess2 = FILE_MAP_READ | FILE_MAP_WRITE;
			
			fileMapping = CreateFileMapping2(file, nullptr, desiredAccess2, PAGE_READWRITE, 0, size, nullptr, nullptr, 0);
			if (!fileMapping) { return false; }

			data = MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, size);
			if (!data) { return false; }

			return true;
		}

		byte* GetData() {
			return static_cast<byte*>(data);
		}

		void Resize(const uint64 newSize)
		{
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