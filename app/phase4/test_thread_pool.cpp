#include "inc/AsyncLogging/ThreadPool.hpp"

#include <iostream>
#include <chrono>
#include <vector>

int main()
{
    std::cout << "=== ThreadPool Test ===" << std::endl;

    // Create thread pool with 4 worker threads
    async_logging::ThreadPool pool(4);

    std::cout << "\n--- Test 1: Simple Tasks ---" << std::endl;

    // Enqueue simple tasks
    for (int i = 1; i <= 8; ++i)
    {
        pool.enqueueTask([i]() {
            std::cout << "[Task " << i << "] Running on thread " 
                      << std::this_thread::get_id() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "[Task " << i << "] Completed" << std::endl;
        });
    }

    // Wait for tasks to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "\n--- Test 2: Tasks with Return Values ---" << std::endl;

    // Enqueue tasks that return values
    std::vector<std::future<int>> results;

    for (int i = 1; i <= 5; ++i)
    {
        auto future = pool.enqueue([i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            return i * i;
        });
        results.push_back(std::move(future));
    }

    // Get results
    std::cout << "Squares: ";
    for (auto& future : results)
    {
        std::cout << future.get() << " ";
    }
    std::cout << std::endl;

    std::cout << "\n--- Test 3: High Load ---" << std::endl;

    std::atomic<int> counter{0};

    for (int i = 0; i < 100; ++i)
    {
        pool.enqueueTask([&counter]() {
            ++counter;
        });
    }

    // Wait for all tasks
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::cout << "Counter value: " << counter.load() << " (expected: 100)" << std::endl;

    std::cout << "\n=== ThreadPool Test Complete ===" << std::endl;

    return 0;
}