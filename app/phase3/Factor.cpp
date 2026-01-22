// main.cpp
#include <iostream>
#include "inc/Formatter/Enums.hpp"
#include "inc/Formatter/Policies.hpp"
#include "inc/Formatter/Parser.hpp"
#include "inc/Formatter/magic_enum.hpp"
#include "inc/Formatter/LogSinkFactory.hpp"
#include "inc/Formatter/LogManagerBuilder.hpp"
#include "inc/logging/LogMessage.hpp"  // Add this include!

int main() {
    using namespace logging;
    
    std::cout << "Creating console sink..." << std::endl;
    auto consoleSink = LogSinkFactory::create(LogSinkType_enum::CONSOLE);
    
    if (!consoleSink) {
        std::cerr << "ERROR: Failed to create console sink!" << std::endl;
        return 1;
    }
    
    std::cout << "Creating file sink..." << std::endl;
    auto fileSink = LogSinkFactory::create(LogSinkType_enum::FILE, "app.log");
    
    if (!fileSink) {
        std::cerr << "ERROR: Failed to create file sink!" << std::endl;
        return 1;
    }
    
    std::cout << "Creating log messages..." << std::endl;
    LogMessage consoleMsg("MyApp", Context::CPU, 50);
    LogMessage fileMsg("MyApp", Context::GPU, 80);
    
    std::cout << "Writing to console sink..." << std::endl;
    consoleSink->write(consoleMsg);
    
    std::cout << "Writing to file sink..." << std::endl;
    fileSink->write(fileMsg);
    
    std::cout << "Done!" << std::endl;
    return 0;
}