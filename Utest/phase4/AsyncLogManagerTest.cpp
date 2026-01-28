#include <gtest/gtest.h>
#include "inc/AsyncLogging/AsyncLogManager.hpp"
#include "inc/logging/ILogSink.hpp"
#include "inc/logging/LogMessage.hpp"
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>

namespace async_logging
{
namespace test
{

// Mock Sink for testing
class MockSink : public logging::ILogSink
{
public:
    mutable std::mutex mutex;
    std::vector<std::string> messages;
    std::atomic<int> writeCount{0};
    
    void write(const logging::LogMessage& msg) const override
    {
        std::lock_guard<std::mutex> lock(mutex);
        const_cast<MockSink*>(this)->messages.push_back(msg.getText());
        const_cast<MockSink*>(this)->writeCount.fetch_add(1);
    }
    
    int getWriteCount() const
    {
        return writeCount.load();
    }
    
    std::vector<std::string> getMessages() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return messages;
    }
};

// ============== Constructor Tests ==============

TEST(AsyncLogManagerTest, ConstructorInitializesCorrectly)
{
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(std::make_shared<MockSink>());
    
    AsyncLogManager manager("TestApp", std::move(sinks), 10);
    
    EXPECT_FALSE(manager.isRunning());
}

TEST(AsyncLogManagerTest, ConstructorWithThreadPool)
{
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(std::make_shared<MockSink>());
    
    AsyncLogManager manager("TestApp", std::move(sinks), 10, true, 4);
    
    EXPECT_FALSE(manager.isRunning());
}

// ============== Start/Stop Tests ==============

TEST(AsyncLogManagerTest, StartAndStop)
{
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(std::make_shared<MockSink>());
    
    AsyncLogManager manager("TestApp", std::move(sinks), 10);
    
    manager.start();
    EXPECT_TRUE(manager.isRunning());
    
    manager.stop();
    EXPECT_FALSE(manager.isRunning());
}

TEST(AsyncLogManagerTest, MultipleStartsAreIdempotent)
{
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(std::make_shared<MockSink>());
    
    AsyncLogManager manager("TestApp", std::move(sinks), 10);
    
    manager.start();
    manager.start();  // Should not cause issues
    manager.start();
    
    EXPECT_TRUE(manager.isRunning());
    
    manager.stop();
}

TEST(AsyncLogManagerTest, MultipleStopsAreIdempotent)
{
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(std::make_shared<MockSink>());
    
    AsyncLogManager manager("TestApp", std::move(sinks), 10);
    
    manager.start();
    manager.stop();
    manager.stop();  // Should not cause issues
    manager.stop();
    
    EXPECT_FALSE(manager.isRunning());
}

// ============== Logging Tests ==============

TEST(AsyncLogManagerTest, LogBeforeStartFails)
{
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(std::make_shared<MockSink>());
    
    AsyncLogManager manager("TestApp", std::move(sinks), 10);
    
    logging::LogMessage msg("Test", logging::Context::CPU, 50);
    
    EXPECT_FALSE(manager.log(std::move(msg)));
}

TEST(AsyncLogManagerTest, LogAfterStartSucceeds)
{
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(std::make_shared<MockSink>());
    
    AsyncLogManager manager("TestApp", std::move(sinks), 10);
    manager.start();
    
    logging::LogMessage msg("Test", logging::Context::CPU, 50);
    
    EXPECT_TRUE(manager.log(std::move(msg)));
    
    manager.stop();
}

TEST(AsyncLogManagerTest, LogAfterStopFails)
{
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(std::make_shared<MockSink>());
    
    AsyncLogManager manager("TestApp", std::move(sinks), 10);
    manager.start();
    manager.stop();
    
    logging::LogMessage msg("Test", logging::Context::CPU, 50);
    
    EXPECT_FALSE(manager.log(std::move(msg)));
}

// ============== Message Delivery Tests ==============

TEST(AsyncLogManagerTest, MessagesDeliveredToSink)
{
    auto mockSink = std::make_shared<MockSink>();
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(mockSink);
    
    AsyncLogManager manager("TestApp", std::move(sinks), 10);
    manager.start();
    
    for (int i = 0; i < 5; ++i)
    {
        logging::LogMessage msg("Test", logging::Context::CPU, static_cast<uint8_t>(i * 20));
        manager.log(std::move(msg));
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    manager.stop();
    
    EXPECT_EQ(mockSink->getWriteCount(), 5);
}

TEST(AsyncLogManagerTest, MessagesDeliveredToMultipleSinks)
{
    auto mockSink1 = std::make_shared<MockSink>();
    auto mockSink2 = std::make_shared<MockSink>();
    
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(mockSink1);
    sinks.push_back(mockSink2);
    
    AsyncLogManager manager("TestApp", std::move(sinks), 10);
    manager.start();
    
    for (int i = 0; i < 5; ++i)
    {
        logging::LogMessage msg("Test", logging::Context::CPU, static_cast<uint8_t>(i * 20));
        manager.log(std::move(msg));
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    manager.stop();
    
    EXPECT_EQ(mockSink1->getWriteCount(), 5);
    EXPECT_EQ(mockSink2->getWriteCount(), 5);
}

// ============== ThreadPool Mode Tests ==============

TEST(AsyncLogManagerTest, ThreadPoolModeDeliverMessages)
{
    auto mockSink = std::make_shared<MockSink>();
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(mockSink);
    
    AsyncLogManager manager("TestApp", std::move(sinks), 10, true, 4);
    manager.start();
    
    for (int i = 0; i < 10; ++i)
    {
        logging::LogMessage msg("Test", logging::Context::CPU, static_cast<uint8_t>(i * 10));
        manager.log(std::move(msg));
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    manager.stop();
    
    EXPECT_EQ(mockSink->getWriteCount(), 10);
}

// ============== Concurrent Logging Tests ==============

TEST(AsyncLogManagerTest, ConcurrentLogging)
{
    auto mockSink = std::make_shared<MockSink>();
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(mockSink);
    
    AsyncLogManager manager("TestApp", std::move(sinks), 100);
    manager.start();
    
    const int numThreads = 4;
    const int messagesPerThread = 25;
    std::vector<std::thread> threads;
    
    for (int t = 0; t < numThreads; ++t)
    {
        threads.emplace_back([&manager, t, messagesPerThread]() {
            for (int i = 0; i < messagesPerThread; ++i)
            {
                logging::LogMessage msg("Thread" + std::to_string(t), 
                                       logging::Context::CPU, 
                                       static_cast<uint8_t>(i % 100));
                manager.log(std::move(msg));
            }
        });
    }
    
    for (auto& t : threads)
    {
        t.join();
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    manager.stop();
    
    EXPECT_EQ(mockSink->getWriteCount(), numThreads * messagesPerThread);
}

// ============== Buffer Full Test ==============

TEST(AsyncLogManagerTest, BufferFullBehavior)
{
    auto mockSink = std::make_shared<MockSink>();
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(mockSink);
    
    // Small buffer
    AsyncLogManager manager("TestApp", std::move(sinks), 5);
    manager.start();
    
    // Log many messages quickly
    int successCount = 0;
    for (int i = 0; i < 100; ++i)
    {
        logging::LogMessage msg("Test", logging::Context::CPU, static_cast<uint8_t>(i % 100));
        if (manager.log(std::move(msg)))
        {
            successCount++;
        }
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    manager.stop();
    
    // All messages that were logged should be delivered
    EXPECT_EQ(mockSink->getWriteCount(), successCount);
}

// ============== Add Sink Test ==============

TEST(AsyncLogManagerTest, AddSinkDynamically)
{
    auto mockSink1 = std::make_shared<MockSink>();
    
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(mockSink1);
    
    AsyncLogManager manager("TestApp", std::move(sinks), 10);
    
    auto mockSink2 = std::make_shared<MockSink>();
    manager.addSink(mockSink2);
    
    manager.start();
    
    logging::LogMessage msg("Test", logging::Context::CPU, 50);
    manager.log(std::move(msg));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    manager.stop();
    
    EXPECT_EQ(mockSink1->getWriteCount(), 1);
    EXPECT_EQ(mockSink2->getWriteCount(), 1);
}

} // namespace test
} // namespace async_logging