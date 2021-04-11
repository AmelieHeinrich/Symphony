#include "ThreadPool.h"

namespace symphony
{
	ThreadPool::ThreadPool(const std::uint_fast32_t& threadCount)
		: ThreadCount(std::max<std::uint_fast32_t>(threadCount, 1)), Threads(new std::thread[std::max<std::uint_fast32_t>(threadCount, 1)])
	{
		CreateThreads();
	}

	ThreadPool::~ThreadPool()
	{
		WaitForTasks();
		m_Running = false;
		DestroyThreads();
	}

    void ThreadPool::Reset(const std::uint_fast32_t& threadCount)
    {
        WaitForTasks();
        m_Running = false;
        DestroyThreads();
        ThreadCount = std::max<std::uint_fast32_t>(threadCount, 1);
        Threads.reset(new std::thread[std::max<std::uint_fast32_t>(threadCount, 1)]);
        m_Running = true;
        CreateThreads();
    }

    void ThreadPool::WaitForTasks()
    {
        while (m_TasksWaiting != 0)
        {
            std::this_thread::yield();
        }
    }

    void ThreadPool::CreateThreads()
    {
        for (std::uint_fast32_t i = 0; i < ThreadCount; i++)
        {
            Threads[i] = std::thread(&ThreadPool::Worker, this);
        }
    }

    void ThreadPool::DestroyThreads()
    {
        for (std::uint_fast32_t i = 0; i < ThreadCount; i++)
        {
            Threads[i].join();
        }
    }

    bool ThreadPool::PopTask(std::function<void()>& task)
    {
        const std::scoped_lock lock(QueueMutex);
        if (Tasks.empty())
            return false;
        else
        {
            task = std::move(Tasks.front());
            Tasks.pop();
            return true;
        }
    }

    void ThreadPool::Worker()
    {
        while (m_Running)
        {
            std::function<void()> task;
            if (PopTask(task))
            {
                task();
                m_TasksWaiting--;
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }
}