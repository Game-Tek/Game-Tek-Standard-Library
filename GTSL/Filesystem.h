#pragma once

#include "Application.h"
#include "StringCommon.h"
#include "String.hpp"
#include "Flags.h"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#define NO_COMM
#include <Windows.h>
#endif

namespace GTSL {
	class FileQuery {
	public:
		using handle_type = void*;
		
		FileQuery(const StringView query) {
			handle = FindFirstFileA(reinterpret_cast<const char*>(query.GetData()), &findData);
		}
		
		Result<StaticString<256>> operator()() {
			if (!counter) {
				++counter;

				if (reinterpret_cast<uint64>(handle) != ERROR_FILE_NOT_FOUND && handle != INVALID_HANDLE_VALUE) {
					return Result(StaticString<256>(reinterpret_cast<const char8_t*>(findData.cFileName)), true);
				}

				return Result<StaticString<256>>(false);
			} else {
				++counter;

				auto res = FindNextFileA(handle, &findData);

				if (res) {
					return Result(StaticString<256>(reinterpret_cast<const char8_t*>(findData.cFileName)), true);
				}

				FindClose(handle);

				return Result<StaticString<256>>(false);
			}
		}

		[[nodiscard]] uint64 GetFileHash() const {
			return static_cast<uint64>(findData.ftLastWriteTime.dwHighDateTime) << 32ull | findData.ftLastWriteTime.dwLowDateTime;
		}

	private:
		handle_type handle = nullptr;
		uint64 counter = 0u;
		WIN32_FIND_DATAA findData;
	};

	class DirectoryQuery {
	public:
		DirectoryQuery() = default;

		using WatchFilterFlag = Flags<uint16, struct WatchFilter>;

		static constexpr auto CHANGE_FILE_NAME = WatchFilterFlag(1), CHANGE_DIR_NAME = WatchFilterFlag(2), FILE_WRITE = WatchFilterFlag(4);

		DirectoryQuery(const StringView pathToWatch, bool watch_subtree, WatchFilterFlag flags) : watchSubtree(watch_subtree) {
			if (flags & CHANGE_FILE_NAME) { dwNotifyFilter |= FILE_NOTIFY_CHANGE_FILE_NAME; }
			if (flags & CHANGE_DIR_NAME) { dwNotifyFilter |= FILE_NOTIFY_CHANGE_DIR_NAME; }
			if (flags & FILE_WRITE) { dwNotifyFilter |= FILE_NOTIFY_CHANGE_LAST_WRITE; }
			notificationHandle = FindFirstChangeNotificationA(reinterpret_cast<const char*>(pathToWatch.GetData()), watchSubtree, dwNotifyFilter);
		}

		void Watch() {
			WaitForSingleObject(notificationHandle, INFINITE);
			FindNextChangeNotification(notificationHandle);
		}

		void EndQuery() {
			FindCloseChangeNotification(notificationHandle);
			notificationHandle = nullptr;
		}

		enum class FileAction {
			ADDED, REMOVED, MODIFIED, OLD_NAME, NEW_NAME
		};

		bool ReadNotifications(auto&& f) {
			BOOL bWatchSubtree = watchSubtree;

			alignas(DWORD) byte notify_informations[sizeof(FILE_NOTIFY_INFORMATION) * 16];

			DWORD lpBytesReturned = 0;
			auto readResult = ReadDirectoryChangesW(notificationHandle, notify_informations, sizeof(FILE_NOTIFY_INFORMATION) * 16, bWatchSubtree, dwNotifyFilter, &lpBytesReturned, nullptr, nullptr);

			if (!readResult) { return false; }

			uint32 i = 0; 
			while(true) {
				const auto& e = *reinterpret_cast<FILE_NOTIFY_INFORMATION*>(notify_informations + i);

				FileAction action;

				switch (e.Action) {
				case FILE_ACTION_ADDED: action = FileAction::ADDED; break;
				case FILE_ACTION_REMOVED: action = FileAction::REMOVED; break;
				case FILE_ACTION_MODIFIED: action = FileAction::MODIFIED; break;
				case FILE_ACTION_RENAMED_OLD_NAME: action = FileAction::OLD_NAME; break;
				case FILE_ACTION_RENAMED_NEW_NAME: action = FileAction::NEW_NAME; break;
				}

				char8_t convertedPath[256];

				auto bytesWritten = WideCharToMultiByte(CP_UTF8, 0, e.FileName, e.FileNameLength / 2, reinterpret_cast<char*>(convertedPath), 256, nullptr, nullptr);

				f(StringView(Byte(bytesWritten), convertedPath), action);

				if(!e.NextEntryOffset) { break; }

				i += e.NextEntryOffset;
			}

			return true;
		}

		~DirectoryQuery() {
			if(notificationHandle) {
				EndQuery();
			}
		}

	private:
		DWORD dwNotifyFilter = 0; bool watchSubtree = true;
		HANDLE notificationHandle = nullptr;
	};
}
