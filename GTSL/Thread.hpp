#pragma once

#include "Core.hpp"
#include "Delegates.hpp"
#include <tuple>
#include <algorithm>

#if (_WIN32)
#include <Windows.h>
#elif (__linux__)
#include <pthread.h>
#endif

namespace GTSL
{
	class Thread final
	{
	public:
		Thread() = default;
		~Thread() = default;

		template<class ALLOCATOR, typename T, typename... Args>
		Thread(ALLOCATOR allocator, const uint8 threadId, Delegate<void,T> del, Args... args) noexcept
		{
			m_dataSize = static_cast<uint32>(sizeof(FunctionCallData<T, Args...>));
			uint64 allocatedSize;
			allocator.Allocate(m_dataSize, 8, &m_data, &allocatedSize);
			new(m_data) FunctionCallData<T, Args...>(threadId, del, GTSL::ForwardRef<Args>(args)...);

			m_handle = CreateThread(&Thread::LaunchThread<T, Args...>, m_data);
		}

		static uint32 ThisThreadID() const { return m_threadId; }

		enum class Priority : uint8
		{
			LOW,LOW_MID,MID,MID_HIGH,HIGH
		};

		void SetPriority(Priority threadPriority)
		{
#if (_WIN32)
			int32 priority{ THREAD_PRIORITY_NORMAL };

			switch(threadPriority)
			{
			case Priority::LOW: priority = THREAD_PRIORITY_LOWEST; break;
			case Priority::LOW_MID: priority = THREAD_PRIORITY_BELOW_NORMAL; break;
			case Priority::MID: priority = THREAD_PRIORITY_NORMAL; break;
			case Priority::MID_HIGH: priority = THREAD_PRIORITY_ABOVE_NORMAL; break;
			case Priority::HIGH: priority = THREAD_PRIORITY_HIGHEST; break;
			}

			SetThreadPriority(m_handle, priority);
#elif (__linux__)
#endif
		}

		void static SetThreadId(const uint8 id) { m_threadId = id; }

		template<class ALLOCATOR>
		void Join(ALLOCATOR allocator)
		{
			join();
			allocator.Deallocate(m_dataSize, 8, m_data);
		}

		void Detach() noexcept { m_handle = nullptr; }

		[[nodiscard]] uint32 GetId() const
		{
#if (_WIN32)
			return GetThreadId(m_handle);
#elif (__linux__)
			return pthread_self();
#endif
		}

		[[nodiscard]] bool CanBeJoined() const noexcept { return !m_handle; }

		static uint8 ThreadCount()
		{
#if (_WIN32)
			SYSTEM_INFO system_info;
			GetSystemInfo(&system_info);
			return static_cast<uint8>(system_info.dwNumberOfProcessors);
#elif __linux__
			return sysconf(_SC_NPROCESSORS_ONLN);
#endif
		}
		explicit operator bool() const
		{
			return m_handle;
		}

	private:
		void* m_handle{nullptr};
		void* m_data;
		uint32 m_dataSize;
		inline thread_local static uint8 m_threadId = 0;

		template<typename FT, typename... Args>
		struct FunctionCallData
		{
			FunctionCallData(const uint8 tId, Delegate<void,FT> del, Args... args)
				: ThreadId(tId), Delegate(del),Parameters(GTSL::ForwardRef<Args>(args)...)
			{}

			uint8 ThreadId;
			Delegate<void, FT> Delegate;
			std::tuple<Args...> Parameters;
		};

#if (_WIN32)
		template<typename FT, typename... ARGS>
		static unsigned long LaunchThread(void* data)
		{
			FunctionCallData<FT, ARGS...>* functionData = static_cast<FunctionCallData<FT, ARGS...>*>(data);

			m_threadId = functionData->ThreadId;

			Call(functionData->delegate, GTSL::MoveRef(functionData->Parameters));

			return 0;
		}
#elif __linux__
		template<typename FT, typename... ARGS>
		static void* LaunchThread(void* data)
		{
			FunctionCallData<FT, ARGS...>* functionData = static_cast<FunctionCallData<FT, ARGS...>*>(data);

			m_threadId = functionData->ThreadId;

			Call(functionData->delegate, GTSL::MoveRef(functionData->Parameters));

			return 0;
		}
#endif
	};
}