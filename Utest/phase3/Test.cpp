// tests/phase3/EnumsTest.cpp

#include <gtest/gtest.h>
#include "inc/Formatter/Enums.hpp"
#include "inc/Formatter/magic_enum.hpp"

using namespace logging;

// ============== SeverityLvl_enum Tests ==============

class SeverityEnumTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SeverityEnumTest, HasCorrectValues) {
    EXPECT_EQ(static_cast<uint8_t>(SeverityLvl_enum::INFO), 0);
    EXPECT_EQ(static_cast<uint8_t>(SeverityLvl_enum::WARNING), 1);
    EXPECT_EQ(static_cast<uint8_t>(SeverityLvl_enum::CRITICAL), 2);
}

TEST_F(SeverityEnumTest, ComparisonOperators) {
    // CRITICAL > WARNING > INFO
    EXPECT_GT(SeverityLvl_enum::CRITICAL, SeverityLvl_enum::WARNING);
    EXPECT_GT(SeverityLvl_enum::WARNING, SeverityLvl_enum::INFO);
    EXPECT_GT(SeverityLvl_enum::CRITICAL, SeverityLvl_enum::INFO);
    
    EXPECT_LT(SeverityLvl_enum::INFO, SeverityLvl_enum::WARNING);
    EXPECT_LT(SeverityLvl_enum::WARNING, SeverityLvl_enum::CRITICAL);
}

TEST_F(SeverityEnumTest, ToStringConversion) {
    EXPECT_EQ(toString(SeverityLvl_enum::INFO), "INFO");
    EXPECT_EQ(toString(SeverityLvl_enum::WARNING), "WARNING");
    EXPECT_EQ(toString(SeverityLvl_enum::CRITICAL), "CRITICAL");
}

TEST_F(SeverityEnumTest, MagicEnumConversion) {
    auto name = magic_enum::enum_name(SeverityLvl_enum::WARNING);
    EXPECT_EQ(name, "WARNING");
    
    auto value = magic_enum::enum_cast<SeverityLvl_enum>("CRITICAL");
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), SeverityLvl_enum::CRITICAL);
}

TEST_F(SeverityEnumTest, MagicEnumInvalidCast) {
    auto value = magic_enum::enum_cast<SeverityLvl_enum>("INVALID");
    EXPECT_FALSE(value.has_value());
}

// ============== LogSinkType_enum Tests ==============

class LogSinkTypeEnumTest : public ::testing::Test {};

TEST_F(LogSinkTypeEnumTest, HasCorrectValues) {
    EXPECT_EQ(static_cast<uint8_t>(LogSinkType_enum::CONSOLE), 0);
    EXPECT_EQ(static_cast<uint8_t>(LogSinkType_enum::FILE), 1);
    EXPECT_EQ(static_cast<uint8_t>(LogSinkType_enum::SOCKET), 2);
}

TEST_F(LogSinkTypeEnumTest, ToStringConversion) {
    EXPECT_EQ(toString(LogSinkType_enum::CONSOLE), "CONSOLE");
    EXPECT_EQ(toString(LogSinkType_enum::FILE), "FILE");
    EXPECT_EQ(toString(LogSinkType_enum::SOCKET), "SOCKET");
}

// ============== TelemetrySrc_enum Tests ==============

class TelemetrySrcEnumTest : public ::testing::Test {};

TEST_F(TelemetrySrcEnumTest, HasCorrectValues) {
    EXPECT_EQ(static_cast<uint8_t>(TelemetrySrc_enum::CPU), 0);
    EXPECT_EQ(static_cast<uint8_t>(TelemetrySrc_enum::GPU), 1);
    EXPECT_EQ(static_cast<uint8_t>(TelemetrySrc_enum::RAM), 2);
}

TEST_F(TelemetrySrcEnumTest, ToStringConversion) {
    EXPECT_EQ(toString(TelemetrySrc_enum::CPU), "CPU");
    EXPECT_EQ(toString(TelemetrySrc_enum::GPU), "GPU");
    EXPECT_EQ(toString(TelemetrySrc_enum::RAM), "RAM");
}

TEST_F(TelemetrySrcEnumTest, MagicEnumIteration) {
    auto values = magic_enum::enum_values<TelemetrySrc_enum>();
    EXPECT_EQ(values.size(), 3);
}

TEST_F(TelemetrySrcEnumTest, MagicEnumCount) {
    constexpr auto count = magic_enum::enum_count<TelemetrySrc_enum>();
    EXPECT_EQ(count, 3);
}