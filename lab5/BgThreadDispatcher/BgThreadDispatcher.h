#pragma once
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

class BgThreadDispatcher
{
public:
	using Task = std::function<void()>;

	BgThreadDispatcher()
	{
		m_workerThread = std::jthread([this](const std::stop_token& stopToken) { Worker(stopToken); });
	}

	BgThreadDispatcher(const BgThreadDispatcher&) = delete;

	BgThreadDispatcher& operator=(const BgThreadDispatcher&) = delete;

	~BgThreadDispatcher()
	{
		Stop();
	}

	void Dispatch(Task task)
	{
		if (m_stopFlag)
		{
			return;
		}
		{
			std::lock_guard lock(m_queueMutex);
			m_tasks.push(std::move(task));
		}
		//уметь объяснить condition variable
		// какой из потоков будет пробуждён
		m_condition.notify_one();
	}

	void Wait()
	{
		std::unique_lock lock(m_queueMutex);
		m_condition.wait(lock, [this]() { return !m_isWorking && m_tasks.empty(); });
	}

	void Stop()
	{
		m_stopFlag = true;
		m_condition.notify_one();
	}

private:
	void Worker(std::stop_token const& stopToken)
	{
		while (!stopToken.stop_requested())
		{
			Task task;
			std::unique_lock lock(m_queueMutex);
			m_condition.wait(lock, [this]() { return m_stopFlag || !m_tasks.empty(); });

			if (m_stopFlag && m_tasks.empty())
			{
				break;
			}

			task = std::move(m_tasks.front());
			m_tasks.pop();
			m_isWorking = true;

			try
			{
				// задачи без лока можно выполнять
				task();
			}
			catch (const std::exception& e)
			{
				std::cerr << "Exception caught in background task: " << e.what() << std::endl;
			}

			m_isWorking = false;
			if (m_tasks.empty())
			{
				m_condition.notify_all();
			}
		}
	}

	std::queue<Task> m_tasks;
	std::mutex m_queueMutex;
	// скорее ошибка, одна и та же юзается и для уведомления потока о появлении задачи и о том, что задача выполнилась
	std::condition_variable m_condition;
	std::atomic<bool> m_stopFlag = false;
	std::atomic<bool> m_isWorking = false;
	std::jthread m_workerThread;
};
