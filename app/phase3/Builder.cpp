// main.cpp
#include <iostream>
#include "inc/Formatter/Enums.hpp"
#include "inc/Formatter/Policies.hpp"
#include "inc/Formatter/Parser.hpp"
#include "inc/Formatter/magic_enum.hpp"
#include "inc/Formatter/LogSinkFactory.hpp"
#include "inc/Formatter/LogManagerBuilder.hpp"
#include "inc/logging/LogMessage.hpp" // Add this include!

int main()
{
    using namespace logging;

    // Fluent interface - very readable!
    auto manager = LogManagerBuilder()
                       .setAppName("TelemetryApp")
                       .addSink(LogSinkType_enum::CONSOLE)
                       .addSink(LogSinkType_enum::FILE, "telemetry.log")
                       .setBufferSize(100)
                       .build();

    std::cout << "Creating log messages..." << std::endl;
    LogMessage consoleMsg("MyApp", Context::CPU, 50);
    LogMessage fileMsg("MyApp", Context::GPU, 80);
    // Use the manager
    manager->log(consoleMsg);
    manager->log(fileMsg);

    return 0;
}