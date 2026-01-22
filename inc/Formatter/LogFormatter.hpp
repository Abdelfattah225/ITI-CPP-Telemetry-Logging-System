// LogFormatter.hpp
#pragma once

#include <string>
#include <optional>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

#include "LogMessage.hpp"
#include "Enums.hpp"
#include "Policies.hpp"
#include "Parser.hpp"

namespace logging {

template<typename Policy>
class LogFormatter {
private:
    static Context mapContext(TelemetrySrc_enum src) {
        switch (src) {
            case TelemetrySrc_enum::CPU: return Context::CPU;
            case TelemetrySrc_enum::GPU: return Context::GPU;
            case TelemetrySrc_enum::RAM: return Context::RAM;
            default: return Context::CPU;
        }
    }

    
    static Severity mapSeverity(SeverityLvl_enum sev) {
        switch (sev) {
            case SeverityLvl_enum::INFO:     return Severity::INFO;
            case SeverityLvl_enum::WARNING:  return Severity::WARN;
            case SeverityLvl_enum::CRITICAL: return Severity::CRITICAL;
            default: return Severity::INFO;
        }
    }

public:
   
    static std::optional<LogMessage> formatDataToLogMsg(const std::string& raw) {
        
        auto parsedValue = parseFloat(raw);
        
        if (!parsedValue) {
            return std::nullopt;  
        }
        
        float value = *parsedValue;
        
        uint8_t payload = static_cast<uint8_t>(
            std::min(255.0f, std::max(0.0f, value))
        );
        
        
        SeverityLvl_enum policySeverity = Policy::inferSeverity(value);
        
        
        Context context = mapContext(Policy::context);
        Severity severity = mapSeverity(policySeverity);
        
        
        LogMessage msg("TelemetryApp", context, severity, payload);
        
        return msg;
    }
    
    // Generates description
    static std::string msgDescription(float val) {
        std::ostringstream oss;
        oss << toString(Policy::context) 
            << " usage at " 
            << std::fixed << std::setprecision(1) << val 
            << Policy::unit;
        return oss.str();
    }
    
    // Generates current timestamp
    static std::string currentTimeStamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

} // namespace logging