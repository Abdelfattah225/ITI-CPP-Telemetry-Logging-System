#include "LogManager.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"
#include "ILogSink.hpp"

int main()
{
    // Step 1: Create LogManager
    logging::LogManager Manger;

    // Step 2: Add ConsoleSink
    Manger.addSink(std::make_unique<logging::ConsoleSinkImpl>());
    // Step 3: Add FileSink (writes to "log.txt")
    Manger.addSink(std::make_unique<logging::FileSinkImpl>("log.txt"));

    // Step 4: Create some log messages
    // INFO (payload <= 25)
    logging::LogMessage msg1("Navigation", logging::Context::CPU, 15);

    // WARN (payload 26-74)
    logging::LogMessage msg2("Bluetooth", logging::Context::RAM, 50);

    // CRITICAL (payload >= 75)
    logging::LogMessage msg3("MediaPlayer", logging::Context::GPU, 90);

    // Step 5: Log the messages
    Manger.log(msg1);
    Manger.log(msg2);
    Manger.log(msg3);
    // Step 6: Flush to all sinks

    Manger.flush();

    return 0;
}