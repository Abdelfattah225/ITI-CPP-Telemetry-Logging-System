#include "TelemetryParser.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main()
{
    SmartDataHub::TelemetryParser parser;

    // Read multiple times to see CPU usage change
    for (int i = 0; i < 5; ++i)
    {
        std::cout << parser.getCpuString() << std::endl;
        std::cout << parser.getMemString() << std::endl;
        std::cout << "-------------------" << std::endl;

        // Wait 1 second between readings
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}