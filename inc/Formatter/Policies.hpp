#pragma once

#include "Enums.hpp"

#include <type_traits>
namespace logging
{

    struct CpuPolicy
    {
        static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::CPU;
        static constexpr std::string_view unit = "%";
        static constexpr float WARNING = 75.0f;
        static constexpr float CRITICAL = 90.0f;

        static constexpr SeverityLvl_enum inferSeverity(float val) noexcept
        {
            return (val > CRITICAL)  ? SeverityLvl_enum::CRITICAL
                   : (val > WARNING) ? SeverityLvl_enum::WARNING
                                     : SeverityLvl_enum::INFO;
        }
    };
    struct GpuPolicy
    {
        static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::GPU;
        static constexpr std::string_view unit = "%";
        static constexpr float WARNING = 80.0f;
        static constexpr float CRITICAL = 95.0f;

        static constexpr SeverityLvl_enum inferSeverity(float val) noexcept
        {
            return (val > CRITICAL)  ? SeverityLvl_enum::CRITICAL
                   : (val > WARNING) ? SeverityLvl_enum::WARNING
                                     : SeverityLvl_enum::INFO;
        }
    };
    struct RamPolicy
    {
        static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::RAM;
        static constexpr std::string_view unit = "%";
        static constexpr float WARNING = 70.0f;
        static constexpr float CRITICAL = 85.0f;

        static constexpr SeverityLvl_enum inferSeverity(float val) noexcept
        {
            return (val > CRITICAL)  ? SeverityLvl_enum::CRITICAL
                   : (val > WARNING) ? SeverityLvl_enum::WARNING
                                     : SeverityLvl_enum::INFO;
        }
    };

    // Returns the threshold range as a string_view
    // Example: "75.0 - 90.0" for CPU
    template <typename Policy>
    constexpr std::string_view getThresholdDescription()
    {
        // You cannot build strings at compile time easily,
        // so just return a static description
        // This is more of a demonstration
        if constexpr (std::is_same_v<Policy, CpuPolicy>)
        {
            return "75.0 - 90.0";
        }
        else if constexpr (std::is_same_v<Policy, GpuPolicy>)
        {
            return "80.0 - 95.0";
        }
        else if constexpr (std::is_same_v<Policy, RamPolicy>)
        {
            return "70.0 - 85.0";
        }else{
            return "UNKOWN Policy";
        }
    }

    template <typename Policy>
    constexpr bool isValidPolicy()
    {
        return Policy::WARNING < Policy::CRITICAL &&
               Policy::WARNING > 0.0f &&
               Policy::CRITICAL <= 100.0f;
    }

    // Compile-time validation of policies
    static_assert(isValidPolicy<CpuPolicy>(), "CpuPolicy has invalid thresholds");
    static_assert(isValidPolicy<GpuPolicy>(), "GpuPolicy has invalid thresholds");
    static_assert(isValidPolicy<RamPolicy>(), "RamPolicy has invalid thresholds");

}