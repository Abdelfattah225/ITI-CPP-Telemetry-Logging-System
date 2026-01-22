// tests/phase3/LogFormatterTest.cpp

#include <gtest/gtest.h>
#include "inc/Formatter/LogFormatter.hpp"
#include "inc/Formatter/Policies.hpp"
#include "inc/logging/LogMessage.hpp"
#include "inc/Formatter/magic_enum.hpp"
#include <regex>

using namespace logging;

// ============== formatDataToLogMsg Tests ==============

class LogFormatterTest : public ::testing::Test {};


TEST_F(LogFormatterTest, FormatCpuInfo) {
    auto result = LogFormatter<CpuPolicy>::formatDataToLogMsg("50.0");
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().getSeverity(), Severity::INFO);
}

TEST_F(LogFormatterTest, FormatCpuCritical) {
    auto result = LogFormatter<CpuPolicy>::formatDataToLogMsg("95.0");
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().getSeverity(), Severity::CRITICAL);
}

TEST_F(LogFormatterTest, FormatValidRamData) {
    auto result = LogFormatter<RamPolicy>::formatDataToLogMsg("72.0");
    
    ASSERT_TRUE(result.has_value());
    
    const auto& msg = result.value();
    EXPECT_EQ(msg.getContext(), Context::RAM);
    EXPECT_EQ(msg.getSeverity(), Severity::WARN);  // 72 > 70
}

TEST_F(LogFormatterTest, FormatValidGpuData) {
    auto result = LogFormatter<GpuPolicy>::formatDataToLogMsg("96.0");
    
    ASSERT_TRUE(result.has_value());
    
    const auto& msg = result.value();
    EXPECT_EQ(msg.getContext(), Context::GPU);
    EXPECT_EQ(msg.getSeverity(), Severity::CRITICAL);  // 96 > 95
}

TEST_F(LogFormatterTest, FailOnInvalidInput) {
    auto result = LogFormatter<CpuPolicy>::formatDataToLogMsg("invalid");
    EXPECT_FALSE(result.has_value());
}

TEST_F(LogFormatterTest, FailOnEmptyInput) {
    auto result = LogFormatter<CpuPolicy>::formatDataToLogMsg("");
    EXPECT_FALSE(result.has_value());
}


// ============== msgDescription Tests ==============

class MsgDescriptionTest : public ::testing::Test {};

TEST_F(MsgDescriptionTest, CpuDescription) {
    std::string desc = LogFormatter<CpuPolicy>::msgDescription(85.5f);
    
    EXPECT_TRUE(desc.find("CPU") != std::string::npos);
    EXPECT_TRUE(desc.find("85.5") != std::string::npos);
    EXPECT_TRUE(desc.find("%") != std::string::npos);
}

TEST_F(MsgDescriptionTest, RamDescription) {
    std::string desc = LogFormatter<RamPolicy>::msgDescription(72.0f);
    
    EXPECT_TRUE(desc.find("RAM") != std::string::npos);
    EXPECT_TRUE(desc.find("72.0") != std::string::npos);
}

TEST_F(MsgDescriptionTest, GpuDescription) {
    std::string desc = LogFormatter<GpuPolicy>::msgDescription(90.0f);
    
    EXPECT_TRUE(desc.find("GPU") != std::string::npos);
    EXPECT_TRUE(desc.find("90.0") != std::string::npos);
}

// ============== currentTimeStamp Tests ==============

class TimestampTest : public ::testing::Test {};

TEST_F(TimestampTest, ReturnsNonEmptyString) {
    std::string timestamp = LogFormatter<CpuPolicy>::currentTimeStamp();
    EXPECT_FALSE(timestamp.empty());
}

TEST_F(TimestampTest, HasCorrectFormat) {
    std::string timestamp = LogFormatter<CpuPolicy>::currentTimeStamp();
    
    // Format: YYYY-MM-DD HH:MM:SS
    std::regex pattern(R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})");
    EXPECT_TRUE(std::regex_match(timestamp, pattern));
}

TEST_F(TimestampTest, ConsecutiveCallsReturnSimilarTimes) {
    std::string ts1 = LogFormatter<CpuPolicy>::currentTimeStamp();
    std::string ts2 = LogFormatter<CpuPolicy>::currentTimeStamp();
    
    // At least the date part should be the same
    EXPECT_EQ(ts1.substr(0, 10), ts2.substr(0, 10));
}

// ============== Integration Tests ==============

class LogFormatterIntegrationTest : public ::testing::Test {};

TEST_F(LogFormatterIntegrationTest, DifferentPoliciesSameValue) {
    std::string rawData = "72.0";
    
    auto cpuResult = LogFormatter<CpuPolicy>::formatDataToLogMsg(rawData);
    auto gpuResult = LogFormatter<GpuPolicy>::formatDataToLogMsg(rawData);
    auto ramResult = LogFormatter<RamPolicy>::formatDataToLogMsg(rawData);
    
    ASSERT_TRUE(cpuResult.has_value());
    ASSERT_TRUE(gpuResult.has_value());
    ASSERT_TRUE(ramResult.has_value());
    
    // Same value, different severities based on policy
    EXPECT_EQ(cpuResult.value().getSeverity(), Severity::INFO);   // 72 < 75
    EXPECT_EQ(gpuResult.value().getSeverity(), Severity::INFO);   // 72 < 80
    EXPECT_EQ(ramResult.value().getSeverity(), Severity::WARN);   // 72 > 70
}