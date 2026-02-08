#include "TelemetryApp.hpp"
#include "inc/logging/ConsoleSinkImpl.hpp"
#include "inc/logging/FileSinkImpl.hpp"
#include "inc/logging/LogMessage.hpp"
#include "inc/SmartDataHub/FileTelemetrySourceImpl.hpp"

#include <iostream>
#include <csignal>
#include <chrono>

namespace facade
{

    // Global flag for signal handling
    static std::atomic<bool> g_shutdownRequested{false};

    void signalHandler(int signum)
    {
        std::cout << "\n[TelemetryApp] Shutdown signal received (" << signum << ")" << std::endl;
        g_shutdownRequested = true;
    }

    TelemetryApp::TelemetryApp(const std::string& configPath)
    {
        std::cout << "[TelemetryApp] Loading configuration from: " << configPath << std::endl;
        
        // Step 1: Load configuration
        m_config = AppConfig::fromJson(configPath);
        m_config.print();

        // Step 2: Create sinks
        createSinks();

        // Step 3: Create AsyncLogManager
        m_logManager = std::make_unique<async_logging::AsyncLogManager>(
            m_config.appName,
            m_sinks,
            m_config.bufferSize,
            true,  // use thread pool
            m_config.threadPoolSize
        );

        std::cout << "[TelemetryApp] Initialized successfully" << std::endl;
    }

    TelemetryApp::~TelemetryApp()
    {
        if (m_running) {
            stop();
        }
    }

    void TelemetryApp::createSinks()
    {
        // Collect unique sink types from all sources
        bool needConsole = false;
        bool needFile = false;

        for (const auto& [name, srcConfig] : m_config.sources) {
            if (!srcConfig.enabled) continue;
            
            for (const auto& sink : srcConfig.sinks) {
                if (sink == SinkType::CONSOLE) needConsole = true;
                if (sink == SinkType::FILE) needFile = true;
            }
        }

        // Create required sinks
        if (needConsole) {
            m_sinks.push_back(std::make_shared<logging::ConsoleSinkImpl>());
            std::cout << "[TelemetryApp] Created Console sink" << std::endl;
        }
        if (needFile) {
            m_sinks.push_back(std::make_shared<logging::FileSinkImpl>(m_config.logFilePath));
            std::cout << "[TelemetryApp] Created File sink: " << m_config.logFilePath << std::endl;
        }
    }

    void TelemetryApp::start()
    {
        if (m_running) {
            std::cerr << "[TelemetryApp] Already running!" << std::endl;
            return;
        }

        m_running = true;
        g_shutdownRequested = false;

        // Install signal handlers
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);

        // Start AsyncLogManager
        m_logManager->start();
        std::cout << "[TelemetryApp] AsyncLogManager started" << std::endl;

        // Create source threads
        createSourceThreads();

        std::cout << "[TelemetryApp] Application started. Press Ctrl+C to stop." << std::endl;
    }

    void TelemetryApp::createSourceThreads()
    {
        for (const auto& [name, srcConfig] : m_config.sources) {
            if (!srcConfig.enabled) {
                std::cout << "[TelemetryApp] Source '" << name << "' is disabled, skipping" << std::endl;
                continue;
            }

            std::cout << "[TelemetryApp] Starting source thread: " << name << std::endl;
            
            m_sourceThreads.emplace_back(&TelemetryApp::sourceWorker, this, name, srcConfig);
        }
    }

    void TelemetryApp::sourceWorker(const std::string& sourceName, const SourceConfig& config)
    {
        std::cout << "[" << sourceName << "] Worker thread started" << std::endl;

        // Create the appropriate source
        std::unique_ptr<SmartDataHub::ITelemetrySource> source;

        if (config.type == SourceType::FILE) {
            source = std::make_unique<SmartDataHub::FileTelemetrySourceImpl>(config.path);
        } else {
            std::cerr << "[" << sourceName << "] VSOMEIP source not yet integrated, skipping" << std::endl;
            return;
        }

        // Open the source
        if (!source->openSource()) {
            std::cerr << "[" << sourceName << "] Failed to open source!" << std::endl;
            return;
        }

        // Determine context from source name
        logging::Context context = logging::Context::CPU;
        if (sourceName == "RAM") context = logging::Context::RAM;
        else if (sourceName == "GPU") context = logging::Context::GPU;

        // Main reading loop
        while (m_running && !g_shutdownRequested) {
            std::string rawData;
            
            if (source->readSource(rawData)) {
                try {
                    // Parse the value (for /proc/stat, we just use a simple percentage)
                    float value = 0.0f;
                    
                    // Simple parsing - just extract first number
                    size_t pos = rawData.find_first_of("0123456789");
                    if (pos != std::string::npos) {
                        value = std::stof(rawData.substr(pos));
                    }
                    
                    // Clamp to 0-100
                    value = std::min(100.0f, std::max(0.0f, value));
                    uint8_t payload = static_cast<uint8_t>(value);

                    // Create and log message
                    logging::LogMessage msg(sourceName, context, payload);
                    
                    if (!m_logManager->log(std::move(msg))) {
                        std::cerr << "[" << sourceName << "] Failed to log message" << std::endl;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "[" << sourceName << "] Parse error: " << e.what() << std::endl;
                }
            }

            // Sleep for configured rate
            for (int i = 0; i < config.parseRateMs / 100 && m_running && !g_shutdownRequested; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        std::cout << "[" << sourceName << "] Worker thread stopped" << std::endl;
    }

    void TelemetryApp::stop()
    {
        if (!m_running) {
            return;
        }

        std::cout << "[TelemetryApp] Stopping..." << std::endl;
        m_running = false;

        // Wait for all source threads to finish
        for (auto& thread : m_sourceThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        m_sourceThreads.clear();

        // Stop AsyncLogManager
        m_logManager->stop();

        std::cout << "[TelemetryApp] Stopped" << std::endl;
    }

    void TelemetryApp::waitForShutdown()
    {
        while (m_running && !g_shutdownRequested) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        stop();
    }

    bool TelemetryApp::isRunning() const
    {
        return m_running;
    }

    const AppConfig& TelemetryApp::getConfig() const
    {
        return m_config;
    }

} // namespace facade
