#include "Enums.hpp"
#include <iostream>
#include <magic_enum.hpp>

int main() {
    using namespace logging;
    
    // Test toString with magic_enum
    std::cout << "=== toString Tests ===\n";
    std::cout << "CRITICAL: " << toString(SeverityLvl_enum::CRITICAL) << "\n";
    std::cout << "FILE: " << toString(LogSinkType_enum::FILE) << "\n";
    std::cout << "GPU: " << toString(TelemetrySrc_enum::GPU) << "\n";
    
    // Test enum_cast (string to enum)
    std::cout << "\n=== enum_cast Tests ===\n";
    auto severity = magic_enum::enum_cast<SeverityLvl_enum>("WARNING");
    if (severity) {
        std::cout << "Parsed 'WARNING': " << toString(*severity) << "\n";
    }
    
    auto invalid = magic_enum::enum_cast<SeverityLvl_enum>("INVALID");
    if (!invalid) {
        std::cout << "Failed to parse 'INVALID' (expected)\n";
    }
    
    // Test iteration
    std::cout << "\n=== All Severity Levels ===\n";
    for (auto level : magic_enum::enum_values<SeverityLvl_enum>()) {
        std::cout << "  - " << magic_enum::enum_name(level) << "\n";
    }
    
    // Test enum_count
    std::cout << "\n=== Enum Counts ===\n";
    std::cout << "Severity levels: " << magic_enum::enum_count<SeverityLvl_enum>() << "\n";
    std::cout << "Sink types: " << magic_enum::enum_count<LogSinkType_enum>() << "\n";
    std::cout << "Telemetry sources: " << magic_enum::enum_count<TelemetrySrc_enum>() << "\n";
    
    return 0;
}