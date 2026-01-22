// main.cpp
#include <iostream>
#include "inc/Formatter/Enums.hpp"
#include "inc/Formatter/Policies.hpp"
#include "inc/Formatter/Parser.hpp"

using namespace logging;

int main() {
        
    // Compile-time evaluation test
    constexpr float test_val = 85.0f;
    constexpr auto severity = CpuPolicy::inferSeverity(test_val);
    
    // This is computed at COMPILE TIME
    static_assert(severity == SeverityLvl_enum::WARNING, 
                  "85% CPU should be WARNING");
    
    // Compile-time policy validation
    static_assert(isValidPolicy<CpuPolicy>(), "CpuPolicy invalid");
    
    std::cout << "All compile-time checks passed!\n";
    std::cout << "CPU 85% severity: " << toString(severity) << "\n";
    
    // Test that inferSeverity also works at runtime
    float runtime_val = 95.0f;
    auto runtime_severity = CpuPolicy::inferSeverity(runtime_val);
    std::cout << "CPU 95% severity: " << toString(runtime_severity) << "\n";
    
    return 0;
}