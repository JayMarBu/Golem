#include "golpch.h"
#include "ThreadPool.h"

namespace golem
{

	ThreadPool::ThreadPool(std::size_t numThreads)
	{
		Start(numThreads);
	}

	ThreadPool::~ThreadPool()
	{
		Stop();
	}

	void ThreadPool::Enqueue(Task task)
	{
		{
			std::unique_lock<std::mutex> lock{ m_eventMutex };
			m_tasks.emplace(std::move(task));
		}

		m_eventVar.notify_one();
	}

	void ThreadPool::Start(std::size_t numThreads)
	{
		for (int i = 0u; i < numThreads; i++)
		{
			m_threads.emplace_back([=] { 
				while (true)
				{
					Task task;

					{
						std::unique_lock<std::mutex> lock{ m_eventMutex };

						m_eventVar.wait(lock, [=] {return m_stopping || !m_tasks.empty();});

						if (m_stopping && m_tasks.empty())
							break;

						task = std::move(m_tasks.front());
						m_tasks.pop();
					}

					task();
				}
			});
		}
	}

	void ThreadPool::Stop() noexcept
	{
		{
			std::unique_lock<std::mutex> lock{ m_eventMutex };
			m_stopping = true;
		}
		m_eventVar.notify_all();

		for (auto& thread : m_threads )
		{
			thread.join();
		}
	}

}