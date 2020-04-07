#pragma once

#include "Vector.hpp"
#include "Thread.h"
#include "BlockingQueue.h"
#include <future>
#include "Array.hpp"

namespace GTSL
{
	class ThreadPool
	{
	public:
		explicit ThreadPool(const uint32 threads = Thread::ThreadCount()) : m_queues(threads), m_count(threads)
		{
			if (!threads) { throw std::invalid_argument("Invalid thread count!"); }

			auto worker = [this](auto i)
			{
				while (true)
				{
					Proc f;
					for (auto n = 0; n < m_count * K; ++n)
						if (m_queues[(i + n) % m_count].try_pop(f))
							break;
					if (!f && !m_queues[i].Pop(f))
						break;
					f();
				}
			};

			for (auto i = 0; i < threads; ++i)
				m_threads.EmplaceBack(worker, i);
		}

		~ThreadPool()
		{
			for (auto& queue : m_queues)
				queue.done();
			for (auto& thread : m_threads)
				thread.Join();
		}

		template<typename F, typename... Args>
		void EnqueueWork(F&& f, Args&&... args)
		{
			auto work = [p = std::forward<F>(f), t = std::make_tuple(std::forward<Args>(args)...)]() { std::apply(p, t); };
			auto i = m_index++;

			for (auto n = 0; n < m_count * K; ++n)
				if (m_queues[(i + n) % m_count].try_push(work))
					return;

			m_queues[i % m_count].push(std::move(work));
		}

		template<typename F, typename... Args>
		[[nodiscard]] auto EnqueueTask(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
		{
			using task_return_type = std::invoke_result_t<F, Args...>;
			using task_type = std::packaged_task<task_return_type()>;

			auto task = std::make_shared<task_type>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
			auto work = [task]() { (*task)(); };
			auto result = task->get_future();
			auto i = m_index++;

			for (auto n = 0; n < m_count * K; ++n)
				if (m_queues[(i + n) % m_count].try_push(work))
					return result;

			m_queues[i % m_count].PushBack(std::move(work));

			return result;
		}

	private:
		using Proc = Delegate<void()>;
		Vector<blocking_queue<Proc>> m_queues;

		Array<Thread, 64, uint8> m_threads;

		const unsigned int m_count;
		std::atomic_uint m_index = 0;

		inline static constexpr uint32 K = 2;
	};
}
