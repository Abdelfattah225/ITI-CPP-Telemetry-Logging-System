#pragma once

#include <memory>
#include <string>
#include "ILogSink.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"
#include "Enums.hpp"

namespace logging {

class LogSinkFactory {
public:
    LogSinkFactory() = delete;
    
    // Factory method
    static std::unique_ptr<ILogSink> create(LogSinkType_enum type);
    
    // Factory method - with configuration (file path)
    static std::unique_ptr<ILogSink> create(LogSinkType_enum type, 
                                            const std::string& config);
};

} // namespace logging