#include "LogManager.hpp"

namespace logging
{

LogManager::LogManager(const std::string& name, std::vector<std::unique_ptr<ILogSink>> sinks, size_t bufferSize)
    : name{name}
    , sinks{std::move(sinks)}
    , m_buffer{bufferSize}
{
}

void LogManager::addSink(std::unique_ptr<ILogSink> sink)
{
    sinks.push_back(std::move(sink));
}

bool LogManager::log(LogMessage msg)
{
    return m_buffer.tryPush(std::move(msg));
}

void LogManager::flush()
{
    while (!m_buffer.isEmpty())
    {
        auto optMsg = m_buffer.tryPop();
        if (optMsg.has_value())
        {
            for (const auto& sink : sinks)
            {
                sink->write(optMsg.value());
            }
        }
    }
}

}