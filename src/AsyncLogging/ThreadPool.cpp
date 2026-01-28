#include "inc/AsyncLogging/ThreadPool.hpp"
#include <iostream>

namespace async_logging
{

ThreadPool::ThreadPool(std::size_t numThreads)
    : m_stop{false}
{
    for (std::size_t i = 0; i < numThreads; ++i)
    {
        m_workers.emplace_back(&ThreadPool::workerLoop, this);
    }

    std::cout << "[ThreadPool] Created with " << numThreads << " threads" << std::endl;
}

ThreadPool::~ThreadPool()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stop.store(true);
    }

    m_condition.notify_all();

    for (std::thread& worker : m_workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }

    std::cout << "[ThreadPool] Destroyed, all threads joined" << std::endl;
}

void ThreadPool::workerLoop()
{
    while (true)
    {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_condition.wait(lock, [this] {
                return m_stop.load() || !m_tasks.empty();
            });

            if (m_stop.load() && m_tasks.empty())
            {
                return;
            }

            task = std::move(m_tasks.front());
            m_tasks.pop();
        }

        task();
    }
}

void ThreadPool::enqueueTask(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_stop.load())
        {
            throw std::runtime_error("Cannot enqueue on stopped ThreadPool");
        }

        m_tasks.emplace(std::move(task));
    }

    m_condition.notify_one();
}

std::size_t ThreadPool::getThreadCount() const
{
    return m_workers.size();
}

std::size_t ThreadPool::getPendingTaskCount()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_tasks.size();
}

} // namespace async_logging