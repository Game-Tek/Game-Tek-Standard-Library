#include "GTSL/Thread.h"

#include <Windows.h>

void* GTSL::Thread::createThread(void* function, void* data) noexcept { return CreateThread(0, 0, static_cast<unsigned long(*)(void*)>(function), data, 0, nullptr); }

GTSL::uint32 GTSL::Thread::ThisTreadID() noexcept { return GetCurrentThreadId(); }

void GTSL::Thread::SetPriority(const Priority threadPriority) const noexcept
{
	int32 priority{ THREAD_PRIORITY_NORMAL };
	switch (threadPriority)
	{
		case Priority::LOW: priority = THREAD_PRIORITY_LOWEST; break;
		case Priority::LOW_MID: priority = THREAD_PRIORITY_BELOW_NORMAL; break;
		case Priority::MID: priority = THREAD_PRIORITY_NORMAL; break;
		case Priority::MID_HIGH: priority = THREAD_PRIORITY_ABOVE_NORMAL; break;
		case Priority::HIGH: priority = THREAD_PRIORITY_HIGHEST; break;
		default: break;
	}

	SetThreadPriority(handle, priority);
	
	return;
}

void GTSL::Thread::Join() const noexcept { WaitForSingleObject(handle, INFINITE); }

void GTSL::Thread::Detach()
{
	//Detach();
}

GTSL::uint32 GTSL::Thread::GetId() const noexcept {	return GetThreadId(handle); }

bool GTSL::Thread::CanBeJoined() const noexcept { return !handle; }
