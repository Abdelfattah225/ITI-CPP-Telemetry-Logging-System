#include "inc/AsyncLogging/AsyncLogManager.hpp"
#include "inc/logging/ConsoleSinkImpl.hpp"
#include "inc/logging/FileSinkImpl.hpp"
#include "inc/logging/LogMessage.hpp"

#include <iostream>
#include <thread>
#include <chrono>

int main()
{
    // Create sinks
    std::vector<std::unique_ptr<logging::ILogSink>> sinks;
    sinks.push_back(std::make_unique<logging::ConsoleSinkImpl>());
    sinks.push_back(std::make_unique<logging::FileSinkImpl>("async_log.txt"));

    // Create AsyncLogManager with buffer capacity of 10
    async_logging::AsyncLogManager asyncManager("TestApp", std::move(sinks), 10);

    // Start the background worker thread
    asyncManager.start();

    std::cout << "=== AsyncLogManager Started ===" << std::endl;

    // Simulate multiple producers logging messages
    std::thread producer1([&asyncManager]() {
        for (int i = 1; i <= 5; ++i)
        {
            // Using 3-arg constructor: (app_name, context, payload)
            // Severity is auto-assigned based on payload value
            uint8_t payload = static_cast<uint8_t>(i * 10);  // 10, 20, 30, 40, 50
            logging::LogMessage msg("Producer1", logging::Context::CPU, payload);

            asyncManager.log(std::move(msg));
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    std::thread producer2([&asyncManager]() {
        for (int i = 1; i <= 5; ++i)
        {
            // Using 4-arg constructor: (app_name, context, severity, payload)
            // Manually specifying severity
            uint8_t payload = static_cast<uint8_t>(i * 15);  // 15, 30, 45, 60, 75
            logging::LogMessage msg("Producer2", logging::Context::RAM, logging::Severity::WARN, payload);

            asyncManager.log(std::move(msg));
            std::this_thread::sleep_for(std::chrono::milliseconds(70));
        }
    });

    std::thread producer3([&asyncManager]() {
        for (int i = 1; i <= 5; ++i)
        {
            // High payload values to trigger CRITICAL severity
            uint8_t payload = static_cast<uint8_t>(75 + i * 5);  // 80, 85, 90, 95, 100
            logging::LogMessage msg("Producer3", logging::Context::GPU, payload);

            asyncManager.log(std::move(msg));
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
        }
    });

    // Wait for producers to finish
    producer1.join();
    producer2.join();
    producer3.join();

    std::cout << "=== Producers Finished ===" << std::endl;

    // Give worker thread time to flush remaining messages
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Stop the manager (waits for worker to finish)
    asyncManager.stop();

    std::cout << "=== AsyncLogManager Stopped ===" << std::endl;

    return 0;
}