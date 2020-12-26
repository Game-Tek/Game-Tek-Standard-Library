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
		BlockingQueue() : done(false), working(false) {}

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
				queue.emplace(GTSL::ForwardRef<T>(item));
			}

			ready.NotifyOne();
		}

		bool TryPush(T&& item)
		{
			if (mutex.TryLock())
			{
				if (working) { mutex.Unlock(); return false; }
				queue.emplace(GTSL::ForwardRef<T>(item));
				mutex.Unlock();
				ready.NotifyOne();
				return true;
			}

			return false;
		}

		bool Pop(T& item)
		{
			Lock lock(mutex);
			
			while (queue.empty() && !done) { ready.Wait(mutex); }
			if (queue.empty()) { return false; }
			item = queue.front();
			queue.pop();
			working = true;
			
			return true;
		}

		bool TryPop(T& item)
		{
			if (mutex.TryLock())
			{
				if (queue.empty()) { mutex.Unlock(); return false; }
				item = queue.front();
				queue.pop();
				working = true;
				mutex.Unlock();
				return true;
			}

			return false;
		}

		void Done() noexcept
		{
			Lock lock(mutex);
			working = false;
		}

		void End() noexcept
		{
			{
				Lock lock(mutex); done = true; working = false;
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
		bool done : 4;
		bool working : 4;
	};
}