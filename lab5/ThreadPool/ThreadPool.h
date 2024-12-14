#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
public:
	using Task = std::function<void()>;

	explicit ThreadPool(const unsigned numThreads)
		: m_stopFlag(false)
	{
		for (unsigned i = 0; i < numThreads; ++i)
		{
			m_workers.emplace_back([this](const std::stop_token& stopToken) { Worker(stopToken); });
		}
	}

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	~ThreadPool()
	{
		Stop();
	}

	void Dispatch(Task task)
	{
		{
			std::lock_guard lock(m_queueMutex);
			if (m_stopFlag)
			{
				return;
			}
			m_tasks.push(std::move(task));
		}
		m_condition.notify_one();
	}

	void Wait()
	{
		std::unique_lock lock(m_queueMutex);
		m_condition.wait(lock, [this]() { return m_tasks.empty() && m_activeTasks == 0; });
	}

	void Stop()
	{
		m_stopFlag = true;
		m_condition.notify_all();
	}

private:
	void Worker(const std::stop_token& stopToken)
	{
		while (!stopToken.stop_requested())
		{
			Task task;
			{
				std::unique_lock lock(m_queueMutex);
				m_condition.wait(lock, [this]() { return m_stopFlag || !m_tasks.empty(); });

				if (m_stopFlag && m_tasks.empty())
				{
					return;
				}

				if (!m_tasks.empty())
				{
					task = std::move(m_tasks.front());
					m_tasks.pop();
					m_activeTasks.fetch_add(1);
				}
			}

			if (task)
			{
				try
				{
					task();
				}
				catch (const std::exception& e)
				{
					std::cerr << "Exception caught in thread pool task: " << e.what() << std::endl;
				}
			}

			// хрень, просто --
			m_activeTasks.fetch_sub(1);
			// состояние гонки, вернуть lock лучше будет
			if (m_activeTasks == 0 && m_tasks.empty())
			{
				m_condition.notify_all();
			}
		}
	}

	std::vector<std::jthread> m_workers;
	std::queue<Task> m_tasks;
	std::mutex m_queueMutex;
	std::condition_variable m_condition;
	std::atomic<bool> m_stopFlag;
	std::atomic<int> m_activeTasks = 0;
};