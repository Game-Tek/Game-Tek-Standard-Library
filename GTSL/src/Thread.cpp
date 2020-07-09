#include "GTSL/Thread.h"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

void* GTSL::Thread::createThread(unsigned long(*function)(void*), void* data) noexcept { return CreateThread(0, 0, function, data, 0, nullptr); }

GTSL::uint32 GTSL::Thread::ThisTreadID() noexcept { return GetCurrentThreadId(); }

void GTSL::Thread::SetPriority(const Priority threadPriority) const noexcept
{
	int32 priority{ THREAD_PRIORITY_NORMAL };
	switch (threadPriority)
	{
		case Priority::LOW:			priority = THREAD_PRIORITY_LOWEST;			break;
		case Priority::LOW_MID:		priority = THREAD_PRIORITY_BELOW_NORMAL;	break;
		case Priority::MID:			priority = THREAD_PRIORITY_NORMAL;			break;
		case Priority::MID_HIGH:	priority = THREAD_PRIORITY_ABOVE_NORMAL;	break;
		case Priority::HIGH:		priority = THREAD_PRIORITY_HIGHEST;			break;
		default: break;
	}

	SetThreadPriority(handle, priority);
}

void GTSL::Thread::Join() const noexcept { WaitForSingleObject(handle, INFINITE); }

void GTSL::Thread::Detach() noexcept { handle = nullptr; }

GTSL::uint32 GTSL::Thread::GetId() const noexcept {	return GetThreadId(handle); }

bool GTSL::Thread::CanBeJoined() const noexcept { return !handle; }

GTSL::uint8 GTSL::Thread::ThreadCount()
{
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	return static_cast<uint8>(system_info.dwNumberOfProcessors);
}
