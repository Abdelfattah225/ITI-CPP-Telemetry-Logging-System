#include "inc/AsyncLogging/AsyncLogManager.hpp"
#include "inc/logging/ConsoleSinkImpl.hpp"
#include "inc/logging/FileSinkImpl.hpp"
#include "inc/logging/LogMessage.hpp"
#include "inc/SmartDataHub/FileTelemetrySourceImpl.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <csignal>

// Global flag for graceful shutdown
std::atomic<bool> g_running{true};

void signalHandler(int signum)
{
    std::cout << "\n[Main] Interrupt signal (" << signum << ") received. Shutting down..." << std::endl;
    g_running.store(false);
}

// Telemetry reader thread function
void telemetryReaderThread(
    smart_data_hub::ITelemetrySource& source,
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
                // Parse the raw data to get payload value
                float value = std::stof(rawData);
                uint8_t payload = static_cast<uint8_t>(std::min(100.0f, std::max(0.0f, value)));

                // Create log message (severity auto-assigned based on payload)
                logging::LogMessage msg(sourceName, context, payload);

                // Log asynchronously
                if (!logManager.log(std::move(msg)))
                {
                    std::cerr << "[" << sourceName << "] Failed to log message (buffer full?)" << std::endl;
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
    // Setup signal handler for graceful shutdown (Ctrl+C)
    std::signal(SIGINT, signalHandler);

    std::cout << "=== Async Telemetry Logger Started ===" << std::endl;
    std::cout << "Press Ctrl+C to stop..." << std::endl;

    // Create sinks
    std::vector<std::unique_ptr<logging::ILogSink>> sinks;
    sinks.push_back(std::make_unique<logging::ConsoleSinkImpl>());
    sinks.push_back(std::make_unique<logging::FileSinkImpl>("telemetry_log.txt"));

    // Create AsyncLogManager with buffer capacity of 100
    async_logging::AsyncLogManager asyncManager("TelemetryApp", std::move(sinks), 100);

    // Start the async log manager
    asyncManager.start();

    // Create telemetry sources
    smart_data_hub::FileTelemetrySourceImpl cpuSource("/tmp/cpu_telemetry.txt");
    smart_data_hub::FileTelemetrySourceImpl ramSource("/tmp/ram_telemetry.txt");
    smart_data_hub::FileTelemetrySourceImpl gpuSource("/tmp/gpu_telemetry.txt");

    // Start telemetry reader threads
    std::thread cpuThread(telemetryReaderThread,
                          std::ref(cpuSource),
                          std::ref(asyncManager),
                          "CPU_Monitor",
                          logging::Context::CPU,
                          500);  // Read every 500ms

    std::thread ramThread(telemetryReaderThread,
                          std::ref(ramSource),
                          std::ref(asyncManager),
                          "RAM_Monitor",
                          logging::Context::RAM,
                          700);  // Read every 700ms

    std::thread gpuThread(telemetryReaderThread,
                          std::ref(gpuSource),
                          std::ref(asyncManager),
                          "GPU_Monitor",
                          logging::Context::GPU,
                          600);  // Read every 600ms

    // Wait for threads to finish (when g_running becomes false)
    cpuThread.join();
    ramThread.join();
    gpuThread.join();

    // Stop the async manager
    asyncManager.stop();

    std::cout << "=== Async Telemetry Logger Stopped ===" << std::endl;

    return 0;
}