#pragma once

#include <memory>
#include <vector>
#include <string>
#include "LogManager.hpp"
#include "LogSinkFactory.hpp"

namespace logging {

class LogManagerBuilder {
private:
    std::string m_appName;
    std::vector<std::unique_ptr<ILogSink>> m_sinks;
    size_t m_bufferSize;

public:
    LogManagerBuilder();
    
    LogManagerBuilder& setAppName(const std::string& name);
    LogManagerBuilder& addSink(LogSinkType_enum type);
    LogManagerBuilder& addSink(LogSinkType_enum type, const std::string& config);
    LogManagerBuilder& setBufferSize(size_t size);
    
    std::unique_ptr<LogManager> build();
};

} // namespace logging