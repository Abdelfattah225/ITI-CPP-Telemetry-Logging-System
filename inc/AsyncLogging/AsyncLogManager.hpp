#ifndef ASYNCLOGMANAGER_HPP
#define ASYNCLOGMANAGER_HPP

#include "ThreadSafeRingBuffer.hpp"
#include "ThreadPool.hpp"
#include "inc/logging/ILogSink.hpp"
#include "inc/logging/LogMessage.hpp"

#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <optional>

namespace async_logging
{

class AsyncLogManager
{
private:
    std::string m_name;
    std::vector<std::shared_ptr<logging::ILogSink>> m_sinks;
    ThreadSafeRingBuffer<logging::LogMessage> m_buffer;
    std::thread m_workerThread;
    std::atomic<bool> m_running;
    std::optional<ThreadPool> m_threadPool;
    bool m_useThreadPool;

    void workerFunction();
    void workerFunctionWithPool();

public:
    AsyncLogManager(const std::string& name,
                    std::vector<std::shared_ptr<logging::ILogSink>> sinks,
                    std::size_t bufferCapacity,
                    bool useThreadPool = false,
                    std::size_t poolSize = 4);

    AsyncLogManager(const AsyncLogManager&) = delete;
    AsyncLogManager& operator=(const AsyncLogManager&) = delete;

    AsyncLogManager(AsyncLogManager&&) = delete;
    AsyncLogManager& operator=(AsyncLogManager&&) = delete;

    ~AsyncLogManager();

    void start();
    void stop();
    bool log(logging::LogMessage msg);
    void addSink(std::shared_ptr<logging::ILogSink> sink);
    bool isRunning() const;
};

} // namespace async_logging

#endif // ASYNCLOGMANAGER_HPP