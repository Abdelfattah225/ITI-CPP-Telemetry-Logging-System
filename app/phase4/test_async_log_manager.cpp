#include "inc/AsyncLogging/AsyncLogManager.hpp"
#include "inc/logging/ConsoleSinkImpl.hpp"
#include "inc/logging/FileSinkImpl.hpp"
#include "inc/logging/LogMessage.hpp"

#include <iostream>
#include <thread>
#include <chrono>

int main()
{
    std::cout << "=== Test 1: Without ThreadPool ===" << std::endl;
    {
        std::vector<std::shared_ptr<logging::ILogSink>> sinks;
        sinks.push_back(std::make_shared<logging::ConsoleSinkImpl>());
        sinks.push_back(std::make_shared<logging::FileSinkImpl>("async_log_no_pool.txt"));

        async_logging::AsyncLogManager asyncManager("TestApp", std::move(sinks), 10, false);
        asyncManager.start();

        for (int i = 1; i <= 5; ++i)
        {
            uint8_t payload = static_cast<uint8_t>(i * 20);
            logging::LogMessage msg("NoPool", logging::Context::CPU, payload);
            asyncManager.log(std::move(msg));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        asyncManager.stop();
    }

    std::cout << "\n=== Test 2: With ThreadPool ===" << std::endl;
    {
        std::vector<std::shared_ptr<logging::ILogSink>> sinks;
        sinks.push_back(std::make_shared<logging::ConsoleSinkImpl>());
        sinks.push_back(std::make_shared<logging::FileSinkImpl>("async_log_with_pool.txt"));

        // Enable ThreadPool with 4 threads
        async_logging::AsyncLogManager asyncManager("TestApp", std::move(sinks), 10, true, 4);
        asyncManager.start();

        for (int i = 1; i <= 5; ++i)
        {
            uint8_t payload = static_cast<uint8_t>(i * 20);
            logging::LogMessage msg("WithPool", logging::Context::RAM, payload);
            asyncManager.log(std::move(msg));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        asyncManager.stop();
    }

    std::cout << "\n=== Tests Complete ===" << std::endl;

    return 0;
}