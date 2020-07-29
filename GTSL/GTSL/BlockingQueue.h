#pragma once

#include "Core.h"
#include "ConditionVariable.h"
#include "Mutex.h"
#include <queue>

namespace GTSL
{
	template<typename T>
	class BlockingQueue
	{
	public:
		void Push(const T& item)
		{
			{
				Lock lock(mutex);
				queue.push(item);
			}
			ready.NotifyOne();
		}

		void Push(T&& item)
		{
			{
				Lock lock(mutex);
				queue.emplace(GTSL::MakeForwardReference<T>(item));
			}
			ready.NotifyOne();
		}

		bool TryPush(const T& item)
		{
			{
				if (!mutex.TryLock()) { return false; }

				queue.push(item);
				mutex.Unlock();
			}

			ready.NotifyOne();
			return true;
		}

		bool TryPush(T&& item)
		{
			{
				if (!mutex.TryLock()) { return false; }
				queue.emplace(GTSL::MakeForwardReference<T>(item));
				mutex.Unlock();
			}

			ready.NotifyOne();
			return true;
		}

		bool Pop(T& item)
		{
			Lock lock(mutex);
			
			while (queue.empty() && !done) { ready.Wait(mutex); }
			if (queue.empty()) { return false; }
			item = queue.front();
			queue.pop();
			
			return true;
		}

		bool TryPop(T& item)
		{
			if (!mutex.TryLock()) { return false; }
			if (queue.empty()) { mutex.Unlock(); return false; }
			
			item = queue.front();
			queue.pop();

			mutex.Unlock();

			return true;
		}

		void Done() noexcept
		{
			{
				Lock lock(mutex); done = true;
			}
			ready.NotifyAll();
		}

		[[nodiscard]] bool IsEmpty() const noexcept
		{
			Lock lock(mutex);
			return queue.empty();
		}

		[[nodiscard]] uint32 GetSize() const noexcept
		{
			Lock lock(mutex);
			return queue.size();
		}

	private:
		std::queue<T> queue;
		mutable Mutex mutex;
		ConditionVariable ready;
		bool done = false;
	};
}