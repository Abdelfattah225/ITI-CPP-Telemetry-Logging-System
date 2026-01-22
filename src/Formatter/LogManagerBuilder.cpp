#include "LogManagerBuilder.hpp"
#include "LogSinkFactory.hpp"
namespace logging
{

    LogManagerBuilder::LogManagerBuilder()
    {
    }

    LogManagerBuilder &LogManagerBuilder::setAppName(const std::string &name)
    {
        m_appName = name;
        return *this;
    }
    LogManagerBuilder &LogManagerBuilder::addSink(LogSinkType_enum type)
    {
        auto sink = LogSinkFactory::create(type);
        if (sink)
        {
            m_sinks.push_back(std::move(sink));
        }
        return *this;
    }
    LogManagerBuilder &LogManagerBuilder::addSink(LogSinkType_enum type, const std::string &config)
    {
        auto sink = LogSinkFactory::create(type, config);
        if (sink)
        {
            m_sinks.push_back(std::move(sink));
        }
        return *this;
    }
    LogManagerBuilder &LogManagerBuilder::setBufferSize(size_t size){
          m_bufferSize = size;
        return *this;
    }

    std::unique_ptr<LogManager> LogManagerBuilder::build(){
            // Validation
        if (m_sinks.empty()) {
            // Add default console sink
            m_sinks.push_back(LogSinkFactory::create(LogSinkType_enum::CONSOLE));
        }
        
        // Construct LogManager
        return std::make_unique<LogManager>(
            m_appName,
            std::move(m_sinks),
            m_bufferSize
        );
    }

} // namespace logging