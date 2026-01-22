#pragma once

#include "ILogSink.hpp"
#include "LogMessage.hpp"

#include <vector>
#include <memory>

namespace logging
{

    class LogManager
    {
    private:
        /* data */
        std::string name;
        std::vector<std::unique_ptr<ILogSink>> sinks;
        std::vector<LogMessage> buffer;

    public:
        LogManager(/* args */) = default;
        // Add this constructor:
        LogManager(const std::string &name, std::vector<std::unique_ptr<ILogSink>> sinks, size_t bufferSize);
        void addSink(std::unique_ptr<ILogSink> sink);

        void log(const LogMessage &msg);

        void flush();

        ~LogManager() = default;
    };

}
