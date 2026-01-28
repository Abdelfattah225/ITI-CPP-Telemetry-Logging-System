#include "inc/AsyncLogging/AsyncLogManager.hpp"
#include "inc/logging/ConsoleSinkImpl.hpp"
#include "inc/logging/FileSinkImpl.hpp"
#include "inc/logging/LogMessage.hpp"
#include "inc/SmartDataHub/FileTelemetrySourceImpl.hpp"
#include "inc/SmartDataHub/ITelemetrySource.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <csignal>

std::atomic<bool> g_running{true};

void signalHandler(int signum)
{
    std::cout << "\n[Main] Interrupt signal (" << signum << ") received. Shutting down..." << std::endl;
    g_running.store(false);
}

void telemetryReaderThread(
    SmartDataHub::ITelemetrySource& source,
    async_logging::AsyncLogManager& logManager,
    const std::string& sourceName,
    logging::Context context,
    int readIntervalMs)
{
    std::cout << "[" << sourceName << "] Thread started" << std::endl;

    if (!source.openSource())
    {
        std::cerr << "[" << sourceName << "] Failed to open source" << std::endl;
        return;
    }

    while (g_running.load())
    {
        std::string rawData;
        if (source.readSource(rawData))
        {
            try
            {
                float value = std::stof(rawData);
                uint8_t payload = static_cast<uint8_t>(std::min(100.0f, std::max(0.0f, value)));
                logging::LogMessage msg(sourceName, context, payload);

                if (!logManager.log(std::move(msg)))
                {
                    std::cerr << "[" << sourceName << "] Failed to log message" << std::endl;
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "[" << sourceName << "] Parse error: " << e.what() << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(readIntervalMs));
    }

    std::cout << "[" << sourceName << "] Thread stopped" << std::endl;
}

int main()
{
    std::signal(SIGINT, signalHandler);

    std::cout << "=== Async Telemetry Logger (with ThreadPool) ===" << std::endl;
    std::cout << "Press Ctrl+C to stop..." << std::endl;

    // Create sinks using shared_ptr (required for ThreadPool)
    std::vector<std::shared_ptr<logging::ILogSink>> sinks;
    sinks.push_back(std::make_shared<logging::ConsoleSinkImpl>());
    sinks.push_back(std::make_shared<logging::FileSinkImpl>("telemetry_log.txt"));

    // Create AsyncLogManager WITH ThreadPool (useThreadPool=true, poolSize=4)
    async_logging::AsyncLogManager asyncManager("TelemetryApp", std::move(sinks), 100, true, 4);
    asyncManager.start();

    // Create telemetry sources
    SmartDataHub::FileTelemetrySourceImpl cpuSource("/tmp/cpu_telemetry.txt");
    SmartDataHub::FileTelemetrySourceImpl ramSource("/tmp/ram_telemetry.txt");
    SmartDataHub::FileTelemetrySourceImpl gpuSource("/tmp/gpu_telemetry.txt");

    // Start telemetry reader threads
    std::thread cpuThread(telemetryReaderThread,
                          std::ref(cpuSource),
                          std::ref(asyncManager),
                          "CPU_Monitor",
                          logging::Context::CPU,
                          500);

    std::thread ramThread(telemetryReaderThread,
                          std::ref(ramSource),
                          std::ref(asyncManager),
                          "RAM_Monitor",
                          logging::Context::RAM,
                          700);

    std::thread gpuThread(telemetryReaderThread,
                          std::ref(gpuSource),
                          std::ref(asyncManager),
                          "GPU_Monitor",
                          logging::Context::GPU,
                          600);

    cpuThread.join();
    ramThread.join();
    gpuThread.join();

    asyncManager.stop();

    std::cout << "=== Async Telemetry Logger Stopped ===" << std::endl;

    return 0;
}