#pragma once
#include <string>
#include <string_view>
#include <cstdint>
#include "magic_enum.hpp"

namespace logging
{

    enum class LogSinkType_enum : uint8_t
    {
        CONSOLE,
        FILE,
        SOCKET,
    };

    enum class SeverityLvl_enum : uint8_t
    {
        CRITICAL = 2,
        WARNING = 1,
        INFO = 0,
    };

    enum class TelemetrySrc_enum : uint8_t
    {
        CPU,
        GPU,
        RAM,
    };


    /* using magic enum*/
    template < typename E>
    constexpr std::string_view toString(E value){
        return magic_enum::enum_name(value);
    }


/* Manual */
/*
constexpr std::string_view toString(LogSinkType_enum type)
{
    switch (type)
    {
        case LogSinkType_enum::CONSOLE:
            return "CONSOLE"; // String literal lives forever
        case LogSinkType_enum::FILE:
            return "FILE"; // No allocation
        case LogSinkType_enum::SOCKET:
            return "SOCKET"; // Just returns view
        }

        return "UNKONWN";
    }

    constexpr std::string_view toString(SeverityLvl_enum level)
    {
        switch (level)
        {
        case SeverityLvl_enum::CRITICAL:
        return "CRITICAL"; // String literal lives forever
        case SeverityLvl_enum::WARNING:
            return "WARNING"; // No allocation
        case SeverityLvl_enum::INFO:
        return "INFO"; // Just returns view
    }
        return "UNKNOWN";
    }

    constexpr std::string_view toString(TelemetrySrc_enum src)
    
    {
        switch (src)
        {
            case TelemetrySrc_enum::CPU:
            return "CPU"; // String literal lives forever
            case TelemetrySrc_enum::GPU:
            return "GPU"; // No allocation
            case TelemetrySrc_enum::RAM:
            return "RAM"; // Just returns view
        }
        
        return "UNKONWN";
    }
*/

    // ============== Comparison Operators ==============

    constexpr bool operator<(SeverityLvl_enum lhs, SeverityLvl_enum rhs)
    {
        // Your implementation
        return static_cast<uint8_t>(lhs) < static_cast<uint8_t>(rhs);
    }

    constexpr bool operator>(SeverityLvl_enum lhs, SeverityLvl_enum rhs)
    {
        // Your implementation
        return static_cast<uint8_t>(lhs) > static_cast<uint8_t>(rhs);
    }

} // namespace logging