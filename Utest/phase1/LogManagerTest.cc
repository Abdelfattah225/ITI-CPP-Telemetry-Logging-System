#include <gtest/gtest.h>
#include "logging/LogManager.hpp"
#include "logging/ConsoleSinkImpl.hpp"
#include "logging/FileSinkImpl.hpp"
#include <cstdio>

class LogManagerTest : public ::testing::Test
{
protected:
    std::string testFileName = "manager_test.txt";
    std::streambuf *originalCoutBuffer;
    std::stringstream capturedOutput;

    void SetUp() override
    {
        std::remove(testFileName.c_str());
        originalCoutBuffer = std::cout.rdbuf();
        std::cout.rdbuf(capturedOutput.rdbuf());
    }

    void TearDown() override
    {
        std::cout.rdbuf(originalCoutBuffer);
        std::remove(testFileName.c_str());
    }

    std::string readFileContent(const std::string &filename)
    {
        std::ifstream file(filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
};

// Test: LogManager can be created
TEST_F(LogManagerTest, CanBeCreated)
{

    logging::LogManager Manager;
    SUCCEED();
}
// Test: Can add sink
TEST_F(LogManagerTest, CanAddSink)
{
    logging::LogManager Manager;
    Manager.addSink(std::make_unique<logging::ConsoleSinkImpl>());
    SUCCEED();
}

// Test: Can add multiple sinks
TEST_F(LogManagerTest, CanAddMultipleSinks)
{
    logging::LogManager Manager;
    Manager.addSink(std::make_unique<logging::ConsoleSinkImpl>());
    Manager.addSink(std::make_unique<logging::FileSinkImpl>(testFileName));
    SUCCEED();
}

// Test: Can log message
TEST_F(LogManagerTest, CanLogMessage)
{
    logging::LogManager Manager;
    logging::LogMessage msg1("TestApp", logging::Context::CPU, 75);
    Manager.log(msg1);
    SUCCEED();
}

// Test: Flush writes to console sink
TEST_F(LogManagerTest, FlushWritesToConsoleSink)
{
    logging::LogManager manager;
    manager.addSink(std::make_unique<logging::ConsoleSinkImpl>());

    logging::LogMessage msg("TestApp", logging::Context::CPU, 20);
    manager.log(msg);
    manager.flush();

    std::string output = capturedOutput.str();
    EXPECT_NE(output.find("TestApp"), std::string::npos);
}

// Test: Flush writes to file sink
TEST_F(LogManagerTest, FlushWritesToFileSink)
{
    logging::LogManager manager;
    manager.addSink(std::make_unique<logging::FileSinkImpl>(testFileName));

    logging::LogMessage msg("TestApp", logging::Context::CPU, 20);
    manager.log(msg);
    manager.flush();

    std::string content = readFileContent(testFileName);
    EXPECT_NE(content.find("TestApp"), std::string::npos);
}

// Test: Flush writes to all sinks
TEST_F(LogManagerTest, FlushWritesToAllSinks)
{
    {
        logging::LogManager manager;
        manager.addSink(std::make_unique<logging::ConsoleSinkImpl>());
        manager.addSink(std::make_unique<logging::FileSinkImpl>(testFileName));
        logging::LogMessage msg1("MultiSinkTest1", logging::Context::RAM, 50);
        logging::LogMessage msg2("MultiSinkTest2", logging::Context::CPU, 20);

        manager.log(msg1);
        manager.log(msg2);

        manager.flush();
    }

    // Check console
    std::string consoleOutput = capturedOutput.str();
    EXPECT_NE(consoleOutput.find("MultiSinkTest1"), std::string::npos);

    // Check file
    std::string fileContent = readFileContent(testFileName);
    EXPECT_NE(fileContent.find("MultiSinkTes2"), std::string::npos);
}

// Test: Flush clears buffer
TEST_F(LogManagerTest, FlushClearsBuffer)
{
    logging::LogManager manager;
    manager.addSink(std::make_unique<logging::ConsoleSinkImpl>());

    logging::LogMessage msg("TestApp", logging::Context::CPU, 20);
    manager.log(msg);
    manager.flush();

    capturedOutput.str(""); 

    manager.flush(); 

    std::string output = capturedOutput.str();
    EXPECT_TRUE(output.empty());
}
