// main.cpp
#include <iostream>
#include "inc/Formatter/Enums.hpp"
#include "inc/Formatter/Policies.hpp"
#include "inc/Formatter/Parser.hpp"
#include "inc/Formatter/magic_enum.hpp"
#include "inc/Formatter/LogSinkFactory.hpp"
#include "inc/Formatter/LogManagerBuilder.hpp"
#include "inc/logging/LogMessage.hpp" // Add this include!
#include "inc/Formatter/LogFormatter.hpp"
#include "LogFormatter.hpp"
#include "Policies.hpp"
#include <iostream>

int main() {
    using namespace logging;
    
    std::cout << "=== LogFormatter Tests ===\n\n";
    
    // Test CPU at different levels
    std::cout << "CPU at 50% (should be INFO):\n";
    auto msg1 = LogFormatter<CpuPolicy>::formatDataToLogMsg("50.0");
    if (msg1) std::cout << *msg1 << "\n\n";
    
    std::cout << "CPU at 80% (should be WARNING - Policy threshold is 75):\n";
    auto msg2 = LogFormatter<CpuPolicy>::formatDataToLogMsg("80.0");
    if (msg2) std::cout << *msg2 << "\n\n";
    
    std::cout << "CPU at 95% (should be CRITICAL - Policy threshold is 90):\n";
    auto msg3 = LogFormatter<CpuPolicy>::formatDataToLogMsg("95.0");
    if (msg3) std::cout << *msg3 << "\n\n";
    
    // Test RAM (different thresholds: 70/85)
    std::cout << "RAM at 72% (should be WARNING - Policy threshold is 70):\n";
    auto msg4 = LogFormatter<RamPolicy>::formatDataToLogMsg("72.0");
    if (msg4) std::cout << *msg4 << "\n\n";
    
    // Test GPU (different thresholds: 80/95)
    std::cout << "GPU at 82% (should be WARNING - Policy threshold is 80):\n";
    auto msg5 = LogFormatter<GpuPolicy>::formatDataToLogMsg("82.0");
    if (msg5) std::cout << *msg5 << "\n\n";
    
    // Test invalid input
    std::cout << "Invalid input:\n";
    auto msg6 = LogFormatter<CpuPolicy>::formatDataToLogMsg("invalid");
    if (!msg6) std::cout << "Correctly returned nullopt\n\n";
    
    // Compare: Old way vs New way
    std::cout << "=== Comparison ===\n";
    std::cout << "Old constructor (fixed thresholds 25/75):\n";
    LogMessage oldWay("App", Context::CPU, 50);  // 50 > 25 → WARN
    std::cout << oldWay << "\n\n";
    
    std::cout << "New constructor via LogFormatter (Policy thresholds):\n";
    auto newWay = LogFormatter<CpuPolicy>::formatDataToLogMsg("50.0");
    if (newWay) std::cout << *newWay << "\n";  // 50 < 75 → INFO
    
    return 0;
}