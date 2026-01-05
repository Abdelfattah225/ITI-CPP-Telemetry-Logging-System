#include <gtest/gtest.h>
#include "logging/LogMessage.hpp"

// Test fixture for LogMessage tests
class LogMessageTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
};

// Test: Constructor sets app_name correctly
TEST_F(LogMessageTest, ConstructorSetAppName)
{
    logging::LogMessage logmsg("TestApp", logging::Context::CPU, 20);
    std::string result = logmsg.getAppName();
    EXPECT_EQ(result, "TestApp");
}

// Test: Constructor sets context correctly
TEST_F(LogMessageTest, ConstructorSetContext)
{
    logging::LogMessage logmsg("TestApp", logging::Context::CPU, 20);
    logging::Context result = logmsg.getContext();
    EXPECT_EQ(result, logging::Context::CPU);
}

// Test: Severity is INFO when payload <= 25
TEST_F(LogMessageTest, SeverityIsInfoWhenPayloadLow)
{
    logging::LogMessage msg1("TestApp", logging::Context::CPU, 20);
    logging::LogMessage msg2("TestApp", logging::Context::CPU, 0);

    EXPECT_EQ(msg1.getSeverity(), logging::Severity::INFO);
    EXPECT_EQ(msg2.getSeverity(), logging::Severity::INFO);
}
// Test: Severity is WARN when payload between 26-74
TEST_F(LogMessageTest, SeverityIsWaenWhenPayloadMedium)
{
    logging::LogMessage msg1("TestApp", logging::Context::CPU, 26);
    logging::LogMessage msg2("TestApp", logging::Context::CPU, 50);
    logging::LogMessage msg3("TestApp", logging::Context::CPU, 74);

    EXPECT_EQ(msg1.getSeverity(), logging::Severity::WARN);
    EXPECT_EQ(msg2.getSeverity(), logging::Severity::WARN);
    EXPECT_EQ(msg3.getSeverity(), logging::Severity::WARN);
}
// Test: Severity is CRITICAL when payload >= 75
TEST_F(LogMessageTest, SeverityIsWaenWhenPayloadHigh)
{
    logging::LogMessage msg1("TestApp", logging::Context::CPU, 75);
    logging::LogMessage msg2("TestApp", logging::Context::CPU, 100);

    EXPECT_EQ(msg1.getSeverity(), logging::Severity::CRITICAL);
    EXPECT_EQ(msg2.getSeverity(), logging::Severity::CRITICAL);

}

// Test: getText returns non-empty string
TEST_F(LogMessageTest, GetTextReturnsFormattedString)
{
    logging::LogMessage msg1("TestApp", logging::Context::CPU, 75);
    std::string text = msg1.getText();
    EXPECT_FALSE(text.empty());

    EXPECT_NE(text.find("TestApp"), std::string::npos);
    EXPECT_NE(text.find("75%"), std::string::npos);
    EXPECT_NE(text.find("CRITICAL"), std::string::npos);
    EXPECT_NE(text.find("CPU"), std::string::npos);
}

// Test: Stream operator works
TEST_F(LogMessageTest, StreamOperatorWorks)
{
    logging::LogMessage msg1("TestApp", logging::Context::CPU, 75);
    
    std::stringstream stringstream;
    stringstream << msg1;
    std::string output = stringstream.str();
    EXPECT_NE(output.find("TestApp"), std::string::npos);
}

