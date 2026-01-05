#include <gtest/gtest.h>

#include "logging/LogMessage.hpp"
#include "logging/ILogSink.hpp"
#include "logging/ConsoleSinkImpl.hpp"

class ConsoleSinkTest : public ::testing::Test
{
protected:
    std::streambuf *originalCoutBuffer;
    std::stringstream capturedOutput;

    void SetUp() override
    {
        originalCoutBuffer = std::cout.rdbuf();
        std::cout.rdbuf(capturedOutput.rdbuf());
    }

    void TearDown() override
    {
        std::cout.rdbuf(originalCoutBuffer);
    }
};

// Test: ConsoleSink can be created
TEST_F(ConsoleSinkTest, CanBeCreated)
{
    logging::ConsoleSinkImpl sink;
    SUCCEED();
}

// Test: Write outputs to console
TEST_F(ConsoleSinkTest, WriteOutputsToConsole)
{
    logging::ConsoleSinkImpl sink;
    logging::LogMessage msg("TestApp", logging::Context::CPU, 20);
    sink.write(msg);

    std::string output = capturedOutput.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("TestApp"), std::string::npos);
}
// Test: ConsoleSink inherits from ILogSink
TEST_F(ConsoleSinkTest, ConsoleSinkInheritsFromILogSink)
{
    logging::ConsoleSinkImpl sink;
    logging::ILogSink * sinkptr = &sink;

    logging::LogMessage msg("TestApp", logging::Context::CPU, 20);
    sinkptr->write(msg);

    std::string output = capturedOutput.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("TestApp"), std::string::npos);
}