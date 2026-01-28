#include "AsyncLogging/AsyncLogManager.hpp"
#include <iostream>

namespace async_logging
{

    AsyncLogManager::AsyncLogManager(const std::string &name,
                                     std::vector<std::unique_ptr<logging::ILogSink>> sinks,
                                     std::size_t bufferCapacity)
        : m_name{name}, m_sinks{std::move(sinks)}, m_buffer{bufferCapacity}, m_running{false}
    {
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
        m_workerThread = std::thread(&AsyncLogManager::workerFunction, this);
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
                for (const auto &sink : m_sinks)
                {
                    sink->write(optMsg.value());
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

    void AsyncLogManager::addSink(std::unique_ptr<logging::ILogSink> sink)
    {
        m_sinks.push_back(std::move(sink));
    }

    bool AsyncLogManager::isRunning() const
    {
        return m_running.load();
    }

} // namespace async_logging