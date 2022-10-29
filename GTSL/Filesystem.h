#pragma once

#include "Application.h"
#include "StringCommon.h"
#include "String.hpp"
#include "Flags.h"

#if (_WIN64)
#include <Windows.h>
#undef ERROR
#elif __linux__
#include <glob.h>
#include <sys/inotify.h>
#endif

namespace GTSL {
	class FileQuery {
	public:
		using handle_type = void*;
		
		FileQuery(const StringView query) {
#if (_WIN64)
			auto path = GTSL::Application::ResolvePath(query);
			handle = FindFirstFileA(reinterpret_cast<const char*>(path.c_str()), &findData);
#elif __linux__
			glob64(reinterpret_cast<const char*>(query.GetData()), GLOB_NOSORT, nullptr, &globData);
#endif
		}
		
		Result<StaticString<256>> operator()() {
#if (_WIN64)
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
#elif __linux__
			if(counter != globData.gl_pathc) {
				auto path = GTSL::StaticString<256>(GTSL::StringView((const char8_t*)globData.gl_pathv[counter++]));
				LTrimLast(path, u8'/');
				return Result<StaticString<256>>(GTSL::MoveRef(path), true);
			} else {
				return Result<StaticString<256>>(false);
			}
#endif
		}

		[[nodiscard]] uint64 GetFileHash() const {
#if (_WIN64)
			return static_cast<uint64>(findData.ftLastWriteTime.dwHighDateTime) << 32ull | findData.ftLastWriteTime.dwLowDateTime;
#elif __linux__
            return 0;
#endif
		}

		~FileQuery() {
#if (_WIN64)
#elif __linux__
			globfree64(&globData);
#endif
		}

	private:
		uint64 counter = 0u;

#if (_WIN64)
		handle_type handle = nullptr;
		WIN32_FIND_DATAA findData;
#elif __linux__
		glob64_t globData;
#endif
	};

	class DirectoryQuery {
	public:
		DirectoryQuery() = default;

		using WatchFilterFlag = Flags<uint16, struct WatchFilter>;

		static constexpr auto CREATE_FILE = WatchFilterFlag(1), CHANGE_FILE_NAME = WatchFilterFlag(2), CHANGE_FILE_HASH = WatchFilterFlag(4), DELETE_FILE = WatchFilterFlag(8), CHANGE_DIRECTORY_NAME = WatchFilterFlag(16);

		DirectoryQuery(const StringView path, bool watch_subtree, WatchFilterFlag watch_flags)
#if (_WIN64)
		: watchFilter(watch_flags), watchSubtree(watch_subtree) {
			auto p = GTSL::Application::ResolvePath(path);

			directoryHandle = CreateFileA(reinterpret_cast<const char*>(p.c_str()), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);
			overlapped.hEvent = CreateEventA(nullptr, false, false, nullptr);
#elif __linux__
		{
			handle = inotify_init();
			if(handle == -1) { return; }
			uint32 flags = 0;
			if(watch_flags & CREATE_FILE) { flags |= IN_CREATE; }
			if(watch_flags & DELETE_FILE) { flags |= IN_DELETE; }
			if(watch_flags & CHANGE_DIRECTORY_NAME) { flags |= IN_MODIFY; }
			watch = inotify_add_watch(handle, reinterpret_cast<const char*>(path.GetData()), flags);
#endif
		}

		enum class FileAction {
			ADDED, REMOVED, MODIFIED, OLD_NAME, NEW_NAME
		};

		bool operator()(auto&& f) {
#if (_WIN64)
			BOOL bWatchSubtree = watchSubtree;

			DWORD lpBytesReturned = 0;
			auto result = WaitForSingleObject(directoryHandle, 0);

			if(result == WAIT_OBJECT_0) { //Only if object is unsignaled
				ReadDirectoryChangesW(directoryHandle, notify_informations, sizeof(FILE_NOTIFY_INFORMATION) * 512, bWatchSubtree, makeFlags(watchFilter), &lpBytesReturned, &overlapped, nullptr);
			}

			auto getResult = GetOverlappedResult(directoryHandle, &overlapped, &lpBytesReturned, false);
			auto getError = GetLastError();

			uint32 i = 0;
			while(i != lpBytesReturned) {
				const auto& e = *reinterpret_cast<FILE_NOTIFY_INFORMATION*>(notify_informations + i);

				FileAction action;

				bool skip = false;

				switch (e.Action) {
				case FILE_ACTION_ADDED:
					if(!(watchFilter & CREATE_FILE)) { skip = true; break; }
					action = FileAction::ADDED;
					break;
				case FILE_ACTION_REMOVED:
					if(!(watchFilter & DELETE_FILE)) { skip = true; break; }
					action = FileAction::REMOVED;
					break;
				case FILE_ACTION_MODIFIED:
					if(!(watchFilter & CHANGE_FILE_HASH)) { skip = true; break; }
					action = FileAction::MODIFIED;
					break;
				case FILE_ACTION_RENAMED_OLD_NAME:
					if(!(watchFilter & CHANGE_FILE_NAME)) { skip = true; break; }
					action = FileAction::OLD_NAME;
					break;
				case FILE_ACTION_RENAMED_NEW_NAME:
					if(!(watchFilter & CHANGE_FILE_NAME)) { skip = true; break; }
					action = FileAction::NEW_NAME;
					break;
				}

				if(skip) { if(!e.NextEntryOffset) { ++i; break; } i += e.NextEntryOffset; continue; }

				char8_t convertedPath[512];

				wchar_t tempPath[512];
				GTSL::MemCopy(e.FileNameLength, e.FileName, tempPath);
				GTSL_ASSERT(e.FileNameLength < 512, "");
				tempPath[e.FileNameLength / sizeof(wchar_t)] = L'\0';

				auto bytesWritten = WideCharToMultiByte(CP_UTF8, 0, tempPath, -1, reinterpret_cast<char*>(convertedPath), 512, nullptr, nullptr);

				f(StringView(Byte(bytesWritten), convertedPath), action);

				if(!e.NextEntryOffset) { ++i; break; }

				i += e.NextEntryOffset;
			}

			return static_cast<bool>(i);
#elif __linux__
			timeval time;
			fd_set rfds;
			int ret = -1;

			/* timeout instantly */
			time.tv_sec = 0;
			time.tv_usec = 0;

			/*
			* add the inotify fd to the fd_set -- of course,
			* your application will probably want to add
			* other file descriptors here, too
			*/
			FD_SET (handle, &rfds);

			ret = select(handle + 1, &rfds, nullptr, nullptr, &time);
			if (ret < 0) {
				perror ("select");
			} else if (!ret) {
				return false;
			} else if (FD_ISSET (handle, &rfds)) {
				/* inotify events are available! */
			}

			inotify_event event;

			GTSL::byte buffer[128 * (sizeof(inotify_event) + 64)];

			int len, i = 0;

			len = read(handle, buffer, sizeof(buffer));
			if (len < 0) {
				if (errno == EINTR) {
					/* need to reissue system call */
				}
			} else if (!len) {
				/* BUF_LEN too small? */
			}

			while (i < len) {
				inotify_event* event = nullptr;

				event = (inotify_event*)&buffer[i];

				printf("wd=%d mask=%u cookie=%u len=%u\n",
					event->wd, event->mask,
					event->cookie, event->len);

				if (event->len) {
					printf ("name=%s\n", event->name);
				}

				i += sizeof(inotify_event) + event->len;
			}

			return i;
#endif
		}

		~DirectoryQuery() {
#if (_WIN64)
			if(directoryHandle) {
				CloseHandle(directoryHandle);
			}

			if(overlapped.hEvent) {
				CloseHandle(overlapped.hEvent);
			}
#elif __linux__
			if(handle != -1) {
				close(handle);
			}

			if(watch != -1) {
				close(watch);
			}
#endif
		}

	private:
#if (_WIN64)
		alignas(DWORD) byte notify_informations[sizeof(FILE_NOTIFY_INFORMATION) * 512];
		WatchFilterFlag watchFilter;
		bool watchSubtree = true;
		HANDLE directoryHandle = nullptr, completionPort = nullptr;
		OVERLAPPED overlapped{};

		static DWORD makeFlags(WatchFilterFlag watch_filter) {
			DWORD dwNotifyFilter = 0;

			if (watch_filter & CREATE_FILE) { dwNotifyFilter |= FILE_NOTIFY_CHANGE_FILE_NAME; }
			if (watch_filter & CHANGE_FILE_NAME) { dwNotifyFilter |= FILE_NOTIFY_CHANGE_FILE_NAME; }
			if (watch_filter & DELETE_FILE) { dwNotifyFilter |= FILE_NOTIFY_CHANGE_FILE_NAME; }

			if (watch_filter & CHANGE_DIRECTORY_NAME) { dwNotifyFilter |= FILE_NOTIFY_CHANGE_DIR_NAME; }

			if (watch_filter & CHANGE_FILE_HASH) { dwNotifyFilter |= FILE_NOTIFY_CHANGE_LAST_WRITE; }

			return dwNotifyFilter;
		}
#elif __linux__
		int handle = -1, watch = -1;
#endif
	};
}
