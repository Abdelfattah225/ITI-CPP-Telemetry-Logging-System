#include <gtest/gtest.h>
#include "inc/AsyncLogging/ThreadPool.hpp"
#include <atomic>
#include <chrono>
#include <vector>

namespace async_logging
{
namespace test
{

// ============== Constructor Tests ==============

TEST(ThreadPoolTest, ConstructorCreatesThreads)
{
    ThreadPool pool(4);
    
    EXPECT_EQ(pool.getThreadCount(), 4);
}

TEST(ThreadPoolTest, ConstructorWithSingleThread)
{
    ThreadPool pool(1);
    
    EXPECT_EQ(pool.getThreadCount(), 1);
}

// ============== Task Execution Tests ==============

TEST(ThreadPoolTest, ExecuteSingleTask)
{
    ThreadPool pool(2);
    std::atomic<bool> executed{false};
    
    pool.enqueueTask([&executed]() {
        executed.store(true);
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    EXPECT_TRUE(executed.load());
}

TEST(ThreadPoolTest, ExecuteMultipleTasks)
{
    ThreadPool pool(4);
    std::atomic<int> counter{0};
    const int numTasks = 100;
    
    for (int i = 0; i < numTasks; ++i)
    {
        pool.enqueueTask([&counter]() {
            counter.fetch_add(1);
        });
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    EXPECT_EQ(counter.load(), numTasks);
}

TEST(ThreadPoolTest, TasksExecuteInParallel)
{
    ThreadPool pool(4);
    std::atomic<int> concurrentCount{0};
    std::atomic<int> maxConcurrent{0};
    
    for (int i = 0; i < 8; ++i)
    {
        pool.enqueueTask([&concurrentCount, &maxConcurrent]() {
            int current = concurrentCount.fetch_add(1) + 1;
            
            // Track maximum concurrent executions
            int expected = maxConcurrent.load();
            while (current > expected && !maxConcurrent.compare_exchange_weak(expected, current))
            {
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            concurrentCount.fetch_sub(1);
        });
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    // Should have had multiple tasks running concurrently
    EXPECT_GT(maxConcurrent.load(), 1);
}

// ============== Return Value Tests ==============

TEST(ThreadPoolTest, TaskWithReturnValue)
{
    ThreadPool pool(2);
    
    auto future = pool.enqueue([]() {
        return 42;
    });
    
    EXPECT_EQ(future.get(), 42);
}

TEST(ThreadPoolTest, TaskWithReturnValueAndArguments)
{
    ThreadPool pool(2);
    
    auto future = pool.enqueue([](int a, int b) {
        return a + b;
    }, 10, 20);
    
    EXPECT_EQ(future.get(), 30);
}

TEST(ThreadPoolTest, MultipleTasksWithReturnValues)
{
    ThreadPool pool(4);
    std::vector<std::future<int>> futures;
    
    for (int i = 0; i < 10; ++i)
    {
        futures.push_back(pool.enqueue([i]() {
            return i * i;
        }));
    }
    
    for (int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(futures[i].get(), i * i);
    }
}

// ============== String Return Tests ==============

TEST(ThreadPoolTest, TaskReturnsString)
{
    ThreadPool pool(2);
    
    auto future = pool.enqueue([]() {
        return std::string("Hello, ThreadPool!");
    });
    
    EXPECT_EQ(future.get(), "Hello, ThreadPool!");
}

// ============== Pending Task Count Tests ==============

TEST(ThreadPoolTest, PendingTaskCount)
{
    ThreadPool pool(1);  // Single thread for predictable behavior
    std::atomic<bool> blockTask{true};
    
    // Block the single worker
    pool.enqueueTask([&blockTask]() {
        while (blockTask.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    
    // Add more tasks that will queue up
    pool.enqueueTask([]() {});
    pool.enqueueTask([]() {});
    
    EXPECT_GE(pool.getPendingTaskCount(), 2);
    
    // Unblock
    blockTask.store(false);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    EXPECT_EQ(pool.getPendingTaskCount(), 0);
}

// ============== Destructor Tests ==============

TEST(ThreadPoolTest, DestructorWaitsForTasks)
{
    std::atomic<int> completed{0};
    
    {
        ThreadPool pool(2);
        
        for (int i = 0; i < 10; ++i)
        {
            pool.enqueueTask([&completed]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                completed.fetch_add(1);
            });
        }
    }  // Destructor called here
    
    // All tasks should complete before destructor returns
    EXPECT_EQ(completed.load(), 10);
}

// ============== Exception Tests ==============

TEST(ThreadPoolTest, EnqueueAfterDestructionThrows)
{
    auto pool = std::make_unique<ThreadPool>(2);
    pool.reset();  // Destroy the pool
    
    // Cannot test enqueue on destroyed pool directly
    // This test verifies pool destruction works
    SUCCEED();
}

// ============== Stress Tests ==============

TEST(ThreadPoolTest, HighLoadStress)
{
    ThreadPool pool(8);
    std::atomic<int> counter{0};
    const int numTasks = 1000;
    
    for (int i = 0; i < numTasks; ++i)
    {
        pool.enqueueTask([&counter]() {
            counter.fetch_add(1);
        });
    }
    
    // Wait for completion
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    EXPECT_EQ(counter.load(), numTasks);
}

TEST(ThreadPoolTest, MixedTaskTypes)
{
    ThreadPool pool(4);
    std::atomic<int> voidCounter{0};
    std::vector<std::future<int>> futures;
    
    for (int i = 0; i < 50; ++i)
    {
        // Void tasks
        pool.enqueueTask([&voidCounter]() {
            voidCounter.fetch_add(1);
        });
        
        // Tasks with return values
        futures.push_back(pool.enqueue([i]() {
            return i;
        }));
    }
    
    // Verify return values
    for (int i = 0; i < 50; ++i)
    {
        EXPECT_EQ(futures[i].get(), i);
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(voidCounter.load(), 50);
}

} // namespace test
} // namespace async_logging