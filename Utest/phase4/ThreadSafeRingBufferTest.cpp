#include <gtest/gtest.h>
#include "inc/AsyncLogging/ThreadSafeRingBuffer.hpp"
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

namespace async_logging
{
namespace test
{

// ============== Basic Functionality Tests ==============

TEST(ThreadSafeRingBufferTest, ConstructorInitializesCorrectly)
{
    ThreadSafeRingBuffer<int> buffer(5);
    
    EXPECT_TRUE(buffer.isEmpty());
    EXPECT_FALSE(buffer.isStopped());
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.capacity(), 5);
}

TEST(ThreadSafeRingBufferTest, PushAndPopSingleThread)
{
    ThreadSafeRingBuffer<int> buffer(5);
    
    EXPECT_TRUE(buffer.push(42));
    EXPECT_EQ(buffer.size(), 1);
    
    auto result = buffer.pop();
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 42);
    EXPECT_TRUE(buffer.isEmpty());
}

TEST(ThreadSafeRingBufferTest, FIFOOrderPreserved)
{
    ThreadSafeRingBuffer<int> buffer(5);
    
    for (int i = 1; i <= 5; ++i)
    {
        buffer.push(i);
    }
    
    for (int i = 1; i <= 5; ++i)
    {
        auto result = buffer.pop();
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), i);
    }
}

// ============== Stop Tests ==============

TEST(ThreadSafeRingBufferTest, StopSignalsCorrectly)
{
    ThreadSafeRingBuffer<int> buffer(5);
    
    EXPECT_FALSE(buffer.isStopped());
    
    buffer.stop();
    
    EXPECT_TRUE(buffer.isStopped());
}

TEST(ThreadSafeRingBufferTest, PushFailsAfterStop)
{
    ThreadSafeRingBuffer<int> buffer(5);
    
    buffer.stop();
    
    EXPECT_FALSE(buffer.push(42));
}

TEST(ThreadSafeRingBufferTest, PopReturnsNulloptAfterStopWhenEmpty)
{
    ThreadSafeRingBuffer<int> buffer(5);
    
    buffer.stop();
    
    auto result = buffer.pop();
    EXPECT_FALSE(result.has_value());
}

TEST(ThreadSafeRingBufferTest, PopDrainsBufferAfterStop)
{
    ThreadSafeRingBuffer<int> buffer(5);
    
    buffer.push(1);
    buffer.push(2);
    buffer.push(3);
    
    buffer.stop();
    
    // Should still be able to pop existing items
    EXPECT_EQ(buffer.pop().value(), 1);
    EXPECT_EQ(buffer.pop().value(), 2);
    EXPECT_EQ(buffer.pop().value(), 3);
    
    // Now should return nullopt
    EXPECT_FALSE(buffer.pop().has_value());
}

// ============== Concurrent Tests ==============

TEST(ThreadSafeRingBufferTest, SingleProducerSingleConsumer)
{
    ThreadSafeRingBuffer<int> buffer(10);
    const int numItems = 100;
    std::vector<int> consumed;
    consumed.reserve(numItems);
    
    std::thread producer([&buffer, numItems]() {
        for (int i = 0; i < numItems; ++i)
        {
            buffer.push(i);
        }
        buffer.stop();
    });
    
    std::thread consumer([&buffer, &consumed]() {
        while (true)
        {
            auto item = buffer.pop();
            if (!item.has_value())
            {
                break;
            }
            consumed.push_back(item.value());
        }
    });
    
    producer.join();
    consumer.join();
    
    EXPECT_EQ(consumed.size(), numItems);
    
    // Verify order
    for (int i = 0; i < numItems; ++i)
    {
        EXPECT_EQ(consumed[i], i);
    }
}

TEST(ThreadSafeRingBufferTest, MultipleProducersSingleConsumer)
{
    ThreadSafeRingBuffer<int> buffer(20);
    const int numProducers = 4;
    const int itemsPerProducer = 25;
    std::atomic<int> consumedCount{0};
    
    std::vector<std::thread> producers;
    for (int p = 0; p < numProducers; ++p)
    {
        producers.emplace_back([&buffer, p, itemsPerProducer]() {
            for (int i = 0; i < itemsPerProducer; ++i)
            {
                buffer.push(p * 1000 + i);
            }
        });
    }
    
    std::thread consumer([&buffer, &consumedCount, numProducers, itemsPerProducer]() {
        int expected = numProducers * itemsPerProducer;
        while (consumedCount.load() < expected)
        {
            auto item = buffer.pop();
            if (item.has_value())
            {
                consumedCount.fetch_add(1);
            }
        }
    });
    
    for (auto& p : producers)
    {
        p.join();
    }
    
    // Wait for consumer to finish
    while (consumedCount.load() < numProducers * itemsPerProducer)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    buffer.stop();
    consumer.join();
    
    EXPECT_EQ(consumedCount.load(), numProducers * itemsPerProducer);
}

TEST(ThreadSafeRingBufferTest, SingleProducerMultipleConsumers)
{
    ThreadSafeRingBuffer<int> buffer(20);
    const int numConsumers = 4;
    const int totalItems = 100;
    std::atomic<int> consumedCount{0};
    
    std::thread producer([&buffer, totalItems]() {
        for (int i = 0; i < totalItems; ++i)
        {
            buffer.push(i);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        buffer.stop();
    });
    
    std::vector<std::thread> consumers;
    for (int c = 0; c < numConsumers; ++c)
    {
        consumers.emplace_back([&buffer, &consumedCount]() {
            while (true)
            {
                auto item = buffer.pop();
                if (!item.has_value())
                {
                    break;
                }
                consumedCount.fetch_add(1);
            }
        });
    }
    
    producer.join();
    for (auto& c : consumers)
    {
        c.join();
    }
    
    EXPECT_EQ(consumedCount.load(), totalItems);
}

// ============== Blocking Behavior Tests ==============

TEST(ThreadSafeRingBufferTest, PopBlocksWhenEmpty)
{
    ThreadSafeRingBuffer<int> buffer(5);
    std::atomic<bool> popCompleted{false};
    
    std::thread consumer([&buffer, &popCompleted]() {
        auto result = buffer.pop();  // Should block
        popCompleted.store(true);
    });
    
    // Give consumer time to block
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(popCompleted.load());
    
    // Unblock by pushing
    buffer.push(42);
    
    consumer.join();
    EXPECT_TRUE(popCompleted.load());
}

TEST(ThreadSafeRingBufferTest, PushBlocksWhenFull)
{
    ThreadSafeRingBuffer<int> buffer(2);
    std::atomic<bool> pushCompleted{false};
    
    buffer.push(1);
    buffer.push(2);
    
    std::thread producer([&buffer, &pushCompleted]() {
        buffer.push(3);  // Should block (buffer full)
        pushCompleted.store(true);
    });
    
    // Give producer time to block
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(pushCompleted.load());
    
    // Unblock by popping
    buffer.pop();
    
    producer.join();
    EXPECT_TRUE(pushCompleted.load());
}

TEST(ThreadSafeRingBufferTest, StopUnblocksWaitingThreads)
{
    ThreadSafeRingBuffer<int> buffer(5);
    std::atomic<bool> threadExited{false};
    
    std::thread consumer([&buffer, &threadExited]() {
        auto result = buffer.pop();  // Should block initially
        threadExited.store(true);
    });
    
    // Give consumer time to block
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(threadExited.load());
    
    // Stop should unblock
    buffer.stop();
    
    consumer.join();
    EXPECT_TRUE(threadExited.load());
}

} // namespace test
} // namespace async_logging