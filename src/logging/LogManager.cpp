#include "LogManager.hpp"

namespace logging{

     
    LogManager::LogManager(const std::string &name, std::vector<std::unique_ptr<ILogSink>> sinks, size_t bufferSize) : name{name}, sinks{std::move(sinks)}{
        buffer.reserve(bufferSize);
    }

    void LogManager::addSink(std::unique_ptr<ILogSink> sink){
        sinks.push_back(std::move(sink));
    }
    void LogManager::log(const LogMessage& msg) {
        buffer.push_back(msg);
    }
    void LogManager::flush() {
        for(const auto & msg : buffer){
            for(const auto & sink : sinks){
                sink->write(msg);
            }
        }
        buffer.clear();
    }

}

