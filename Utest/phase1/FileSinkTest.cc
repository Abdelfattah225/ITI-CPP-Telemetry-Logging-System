#include <gtest/gtest.h>

#include "logging/LogMessage.hpp"
#include "logging/ILogSink.hpp"
#include "logging/FileSinkImpl.hpp"

#include <fstream>
#include <cstdio>

class FileSinkTest : public ::testing::Test
{
protected:
    std::string testFileName = "test_log.txt";

    void SetUp() override
    {
        std::remove(testFileName.c_str());
    }

    void TearDown() override
    {
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

// Test: FileSink creates file
TEST_F(FileSinkTest, WritesToFile)
{
    {
        logging::FileSinkImpl sink(testFileName);
        logging::LogMessage msg("TestApp", logging::Context::CPU, 20);
        sink.write(msg);
    }

    std::string content = readFileContent(testFileName);
    EXPECT_FALSE(content.empty());
    EXPECT_NE(content.find("TestApp"), std::string::npos);
}
// Test: FileSink appends multiple messages
TEST_F(FileSinkTest, AppendsMultipleMessages)
{
    {
        logging::FileSinkImpl sink(testFileName);
        logging::LogMessage msg1("APP1", logging::Context::CPU, 20);
        logging::LogMessage msg2("APP2", logging::Context::CPU, 20);

        sink.write(msg1);
        sink.write(msg2);
    }
    std::string content = readFileContent(testFileName);
    EXPECT_NE(content.find("APP1"), std::string::npos);
    EXPECT_NE(content.find("APP2"), std::string::npos);
}

// Test: FileSink inherits from ILogSink
TEST_F(FileSinkTest, FileSinkInheritsFromILogSink)
{
    logging::FileSinkImpl file(testFileName);
    logging::ILogSink *sink = &file;

    logging::LogMessage msg1("APP1", logging::Context::CPU, 20);

    sink->write(msg1);

    SUCCEED();
}