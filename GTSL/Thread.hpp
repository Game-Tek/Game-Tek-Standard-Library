#pragma once

#include "Core.h"
#include "Delegate.hpp"
#include "Tuple.hpp"
#include "Algorithm.hpp"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace GTSL
{	
	class Thread {
	public:
		Thread() = default;

		template<class ALLOCATOR, typename T, typename... ARGS>
		Thread(const ALLOCATOR& allocator, const uint8 threadIdParam, Delegate<T> delegate, ARGS... args) noexcept
		{
			uint64 allocatedSize;
			
			this->dataSize = static_cast<uint32>(sizeof(FunctionCallData<T, ARGS...>));
			
			allocator.Allocate(this->dataSize, 8, &this->data, &allocatedSize);
			new(this->data) FunctionCallData<T, ARGS...>(threadIdParam, delegate, GTSL::ForwardRef<ARGS>(args)...);
			
			this->handle = createThread(&Thread::launchThread<T, ARGS...>, this->data);
		}

		static uint32 ThisTreadID() noexcept { return threadId; }

		enum class Priority : uint8 {
			LOW, LOW_MID, MID, MID_HIGH, HIGH
		};
		void SetPriority(Priority threadPriority) const noexcept {
#if (_WIN64)
			int32 priority{ THREAD_PRIORITY_NORMAL };
			switch (threadPriority)
			{
			case Priority::LOW:			priority = THREAD_PRIORITY_LOWEST;			break;
			case Priority::LOW_MID:		priority = THREAD_PRIORITY_BELOW_NORMAL;	break;
			case Priority::MID:			priority = THREAD_PRIORITY_NORMAL;			break;
			case Priority::MID_HIGH:	priority = THREAD_PRIORITY_ABOVE_NORMAL;	break;
			case Priority::HIGH:		priority = THREAD_PRIORITY_HIGHEST;			break;
			default: return;
			}

			SetThreadPriority(handle, priority);
#elif __linux__
#endif
		}

		void static SetThreadId(const uint8 id) { threadId = id; }
		
		template<class ALLOCATOR>
		void Join(const ALLOCATOR& allocator) noexcept {
			this->join(); //wait first
			allocator.Deallocate(this->dataSize, 8, this->data); //deallocate next
		}
		
		void Detach() noexcept { handle = nullptr; }

		[[nodiscard]] uint32 GetId() const noexcept {
#if (_WIN64)
			return GetThreadId(handle);
#elif __linux__
			return 0;
#endif
		}

		[[nodiscard]] bool CanBeJoined() const noexcept { return !handle; }
		
		static uint8 ThreadCount() {
#if (_WIN64)
			SYSTEM_INFO system_info;
			GetSystemInfo(&system_info);
			return static_cast<uint8>(system_info.dwNumberOfProcessors);
#elif __linux__
#endif
		}
		
	private:
		template<typename FT, typename... ARGS>
		struct FunctionCallData {
			FunctionCallData(const uint8 tId, Delegate<FT> delegate, ARGS&&... args) : ThreadId(tId), delegate(delegate), Parameters(GTSL::ForwardRef<ARGS>(args)...)
			{
			}

			uint8 ThreadId;
			Delegate<FT> delegate;
			Tuple<ARGS...> Parameters;
		};

		void* handle{ nullptr };
		void* data;
		uint32 dataSize;
		
		template<typename FT, typename... ARGS>
		static unsigned long launchThread(void* data) {
			FunctionCallData<FT, ARGS...>* functionData = static_cast<FunctionCallData<FT, ARGS...>*>(data);

			threadId = functionData->ThreadId;
			
			Call(functionData->delegate, GTSL::MoveRef(functionData->Parameters));

			return 0;
		}
		
		static void* createThread(unsigned long(*function)(void*), void* data) noexcept { 
#if (_WIN64)
			return CreateThread(0, 0, function, data, 0, nullptr);
#elif __linux__
#endif
		}

		inline thread_local static uint8 threadId = 0;

		void join() noexcept {
#if (_WIN64)
			WaitForSingleObject(handle, INFINITE); handle = nullptr;
#elif __linux__
#endif
		}
	};
}
