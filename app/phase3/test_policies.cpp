// main.cpp
#include <iostream>
#include "inc/Formatter/Enums.hpp"
#include "inc/Formatter/Policies.hpp"
using namespace logging;

int main()
{

    // Test CpuPolicy
    std::cout << "CPU Tests:\n";
    std::cout << "  Context: " << toString(CpuPolicy::context) << "\n";
    std::cout << "  Unit: " << CpuPolicy::unit << "\n";
    std::cout << "  50% -> " << toString(CpuPolicy::inferSeverity(50.0f)) << "\n"; // INFO
    std::cout << "  80% -> " << toString(CpuPolicy::inferSeverity(80.0f)) << "\n"; // WARNING
    std::cout << "  95% -> " << toString(CpuPolicy::inferSeverity(95.0f)) << "\n"; // CRITICAL

    // Test GpuPolicy
    std::cout << "\nGPU Tests:\n";
    std::cout << "  Context: " << toString(GpuPolicy::context) << "\n";
    std::cout << "  70% -> " << toString(GpuPolicy::inferSeverity(70.0f)) << "\n"; // INFO
    std::cout << "  85% -> " << toString(GpuPolicy::inferSeverity(85.0f)) << "\n"; // WARNING
    std::cout << "  96% -> " << toString(GpuPolicy::inferSeverity(96.0f)) << "\n"; // CRITICAL

    // Test RamPolicy
    std::cout << "\nRAM Tests:\n";
    std::cout << "  Context: " << toString(RamPolicy::context) << "\n";
    std::cout << "  60% -> " << toString(RamPolicy::inferSeverity(60.0f)) << "\n"; // INFO
    std::cout << "  75% -> " << toString(RamPolicy::inferSeverity(75.0f)) << "\n"; // WARNING
    std::cout << "  90% -> " << toString(RamPolicy::inferSeverity(90.0f)) << "\n"; // CRITICAL

    return 0;
}