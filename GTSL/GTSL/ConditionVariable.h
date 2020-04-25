#pragma once

#include <condition_variable>

namespace GTSL
{
	class MultithreadNotification
	{
		std::condition_variable conditionVariable;
		std::mutex mutex;

	public:
		MultithreadNotification();

		void Wait()
		{
			std::unique_lock<std::mutex> lock(mutex);
			conditionVariable.wait(lock);
		}

		void NotifyAll()
		{
			conditionVariable.notify_all();
		}

		void NotifyOne()
		{
			conditionVariable.notify_one();
		}
	};
}
