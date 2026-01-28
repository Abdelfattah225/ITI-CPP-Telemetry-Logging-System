#pragma once

#include "ILogSink.hpp"
#include "LogMessage.hpp"
#include "inc/AsyncLogging/RingBuffer.hpp"
#include <vector>
#include <memory>

namespace logging
{

class LogManager
{
private:
    std::string name;
    std::vector<std::unique_ptr<ILogSink>> sinks;
    async_logging::RingBuffer<LogMessage> m_buffer;

public:
    LogManager(const std::string& name, std::vector<std::unique_ptr<ILogSink>> sinks, size_t bufferSize);

    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;

    LogManager(LogManager&&) = default;
    LogManager& operator=(LogManager&&) = default;

    ~LogManager() = default;

    void addSink(std::unique_ptr<ILogSink> sink);

    bool log(LogMessage msg);

    void flush();
};

}