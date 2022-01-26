#pragma once
#include <thread>
#include <condition_variable>
#include <mutex>
#include <future>
#include "Golem/Events/Event.h"

namespace golem
{
	class ThreadPool
	{
		// Members ********************************************************************************
	public:
		using Task = std::function<void()>;

	private:
		std::vector<std::thread> m_threads;

		std::condition_variable m_eventVar;
		std::mutex m_eventMutex;
		bool m_stopping = false;

		std::queue<Task> m_tasks;

		std::mutex m_gEventMutex;
		std::queue<Event*> m_events;
		// Methods ********************************************************************************
	public:
		explicit ThreadPool(std::size_t numThreads);

		~ThreadPool();

		REMOVE_COPY_CONSTRUCTOR(ThreadPool);

		template<class T>
		auto Enqueue(Task task)->std::future<decltype(task())>;

		void Enqueue(Task task);

		void PollEvents();
		void FireEvent(Event* e);

	private:
		void Start(std::size_t numThreads);
		void Stop() noexcept;
	};

	template<class T>
	auto ThreadPool::Enqueue(Task task)->std::future<decltype(task())>
	{
		auto wrapper = std::make_shared<std::packaged_task<decltype(task())()>>(std::move(task))

		{
			std::unique_lock<std::mutex> lock{ m_eventMutex };
			m_tasks.emplace([=]
			{
				(*wrapper)();
			});
		}

		m_eventVar.notify_one();
		return wrapper->get_future();
	}
}