#include "LogManager.hpp"

namespace logging{


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