#include "LogSinkFactory.hpp"

namespace logging
{

    std::unique_ptr<ILogSink> LogSinkFactory::create(LogSinkType_enum type)
    {
        switch (type)
        {
        case LogSinkType_enum::CONSOLE:
          return  std::make_unique<ConsoleSinkImpl>();
            break;
        case LogSinkType_enum::FILE:
           return std::make_unique<FileSinkImpl>("Default.log");
            break;
        default:
            throw std::runtime_error("This sink not implemented");
            break;
        }
    }
    // Factory method - with configuration (content of file)
    std::unique_ptr<ILogSink> LogSinkFactory::create(LogSinkType_enum type, const std::string &config)
    {
        switch (type)
        {
        case LogSinkType_enum::CONSOLE:
           return std::make_unique<ConsoleSinkImpl>();
            break;
        case LogSinkType_enum::FILE:
           return std::make_unique<FileSinkImpl>(config);
            break;
        default:
            throw std::runtime_error("This sink not implemented");
            break;
        }
    }
}
