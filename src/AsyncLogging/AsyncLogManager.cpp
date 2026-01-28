#include "inc/AsyncLogging/AsyncLogManager.hpp"
#include <iostream>

namespace async_logging
{

AsyncLogManager::AsyncLogManager(const std::string& name,
                                 std::vector<std::shared_ptr<logging::ILogSink>> sinks,
                                 std::size_t bufferCapacity,
                                 bool useThreadPool,
                                 std::size_t poolSize)
    : m_name{name}
    , m_sinks{std::move(sinks)}
    , m_buffer{bufferCapacity}
    , m_running{false}
    , m_useThreadPool{useThreadPool}
{
    if (m_useThreadPool)
    {
        m_threadPool.emplace(poolSize);
    }
}

AsyncLogManager::~AsyncLogManager()
{
    stop();
}

void AsyncLogManager::start()
{
    if (m_running.load())
    {
        return;
    }

    m_running.store(true);

    if (m_useThreadPool)
    {
        m_workerThread = std::thread(&AsyncLogManager::workerFunctionWithPool, this);
    }
    else
    {
        m_workerThread = std::thread(&AsyncLogManager::workerFunction, this);
    }
}

void AsyncLogManager::stop()
{
    if (!m_running.load())
    {
        return;
    }

    m_running.store(false);
    m_buffer.stop();

    if (m_workerThread.joinable())
    {
        m_workerThread.join();
    }
}

void AsyncLogManager::workerFunction()
{
    while (m_running.load() || !m_buffer.isEmpty())
    {
        auto optMsg = m_buffer.pop();

        if (optMsg.has_value())
        {
            for (const auto& sink : m_sinks)
            {
                sink->write(optMsg.value());
            }
        }
    }
}

void AsyncLogManager::workerFunctionWithPool()
{
    while (m_running.load() || !m_buffer.isEmpty())
    {
        auto optMsg = m_buffer.pop();

        if (optMsg.has_value())
        {
            // Copy the message for each sink task
            logging::LogMessage msg = optMsg.value();

            for (const auto& sink : m_sinks)
            {
                // Capture sink and message by value (shared_ptr is cheap to copy)
                m_threadPool->enqueueTask([sink, msg]() {
                    sink->write(msg);
                });
            }
        }
    }
}

bool AsyncLogManager::log(logging::LogMessage msg)
{
    if (!m_running.load())
    {
        return false;
    }

    return m_buffer.push(std::move(msg));
}

void AsyncLogManager::addSink(std::shared_ptr<logging::ILogSink> sink)
{
    m_sinks.push_back(std::move(sink));
}

bool AsyncLogManager::isRunning() const
{
    return m_running.load();
}

} // namespace async_logging