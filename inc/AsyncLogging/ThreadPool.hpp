#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <future>

namespace async_logging
{

class ThreadPool
{
private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;

    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_stop;

    void workerLoop();

public:
    explicit ThreadPool(std::size_t numThreads);

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    ~ThreadPool();

    // Enqueue a task (any callable)
    template <typename F, typename... Args>
    auto enqueue(F&& func, Args&&... args) -> std::future<decltype(func(args...))>;

    // Simple version: enqueue a void() function
    void enqueueTask(std::function<void()> task);

    std::size_t getThreadCount() const;

    std::size_t getPendingTaskCount();
};


template <typename F, typename... Args>
auto ThreadPool::enqueue(F&& func, Args&&... args) -> std::future<decltype(func(args...))>
{
    using ReturnType = decltype(func(args...));

    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<F>(func), std::forward<Args>(args)...)
    );

    std::future<ReturnType> result = task->get_future();

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_stop.load())
        {
            throw std::runtime_error("Cannot enqueue on stopped ThreadPool");
        }

        m_tasks.emplace([task]() { (*task)(); });
    }

    m_condition.notify_one();
    return result;
}

} // namespace async_logging

#endif // THREADPOOL_HPP