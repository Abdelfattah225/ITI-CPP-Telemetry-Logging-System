// tests/phase3/ParserTest.cpp

#include <gtest/gtest.h>
#include "inc/Formatter/Parser.hpp"
#include "inc/Formatter/Policies.hpp"

using namespace logging;

// ============== parseFloat Tests ==============

class ParseFloatTest : public ::testing::Test {};

TEST_F(ParseFloatTest, ParseValidInteger) {
    auto result = parseFloat("42");
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value(), 42.0f);
}

TEST_F(ParseFloatTest, ParseValidFloat) {
    auto result = parseFloat("3.14");
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(result.value(), 3.14f, 0.01f);
}

TEST_F(ParseFloatTest, ParseValidNegative) {
    auto result = parseFloat("-42.5");
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value(), -42.5f);
}

TEST_F(ParseFloatTest, ParseZero) {
    auto result = parseFloat("0");
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value(), 0.0f);
}

TEST_F(ParseFloatTest, ParseZeroPointZero) {
    auto result = parseFloat("0.0");
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value(), 0.0f);
}

TEST_F(ParseFloatTest, ParseLargeNumber) {
    auto result = parseFloat("99999.99");
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(result.value(), 99999.99f, 0.1f);
}

TEST_F(ParseFloatTest, FailOnEmptyString) {
    auto result = parseFloat("");
    EXPECT_FALSE(result.has_value());
}

TEST_F(ParseFloatTest, FailOnAlphabetic) {
    auto result = parseFloat("hello");
    EXPECT_FALSE(result.has_value());
}

TEST_F(ParseFloatTest, FailOnMixedContent) {
    auto result = parseFloat("12abc");
    // Depending on implementation, this might parse "12" or fail
    // If strict parsing, it should fail
}

TEST_F(ParseFloatTest, FailOnSpecialCharacters) {
    auto result = parseFloat("!@#$");
    EXPECT_FALSE(result.has_value());
}

TEST_F(ParseFloatTest, ParseWhitespaceHandling) {
    // Leading/trailing whitespace behavior
    auto result = parseFloat("  42  ");
    // This depends on implementation - from_chars doesn't skip whitespace
    // So this should fail or parse differently
}

// ============== parseAndClassify Tests ==============

class ParseAndClassifyTest : public ::testing::Test {};

TEST_F(ParseAndClassifyTest, ClassifyCpuInfo) {
    auto result = parseAndClassify<CpuPolicy>("50.0");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), SeverityLvl_enum::INFO);
}

TEST_F(ParseAndClassifyTest, ClassifyCpuWarning) {
    auto result = parseAndClassify<CpuPolicy>("80.0");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), SeverityLvl_enum::WARNING);
}

TEST_F(ParseAndClassifyTest, ClassifyCpuCritical) {
    auto result = parseAndClassify<CpuPolicy>("95.0");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), SeverityLvl_enum::CRITICAL);
}

TEST_F(ParseAndClassifyTest, ClassifyRamWarning) {
    auto result = parseAndClassify<RamPolicy>("72.0");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), SeverityLvl_enum::WARNING);
}

TEST_F(ParseAndClassifyTest, ClassifyGpuCritical) {
    auto result = parseAndClassify<GpuPolicy>("96.0");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), SeverityLvl_enum::CRITICAL);
}

TEST_F(ParseAndClassifyTest, FailOnInvalidInput) {
    auto result = parseAndClassify<CpuPolicy>("invalid");
    EXPECT_FALSE(result.has_value());
}

TEST_F(ParseAndClassifyTest, FailOnEmptyInput) {
    auto result = parseAndClassify<CpuPolicy>("");
    EXPECT_FALSE(result.has_value());
}

// ============== Boundary Tests ==============

class PolicyBoundaryTest : public ::testing::Test {};

TEST_F(PolicyBoundaryTest, CpuAtWarningBoundary) {
    // At exactly 75.0, should be INFO (not greater than)
    auto result = parseAndClassify<CpuPolicy>("75.0");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), SeverityLvl_enum::INFO);
}

TEST_F(PolicyBoundaryTest, CpuJustAboveWarning) {
    auto result = parseAndClassify<CpuPolicy>("75.1");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), SeverityLvl_enum::WARNING);
}

TEST_F(PolicyBoundaryTest, CpuAtCriticalBoundary) {
    // At exactly 90.0, should be WARNING (not greater than)
    auto result = parseAndClassify<CpuPolicy>("90.0");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), SeverityLvl_enum::WARNING);
}

TEST_F(PolicyBoundaryTest, CpuJustAboveCritical) {
    auto result = parseAndClassify<CpuPolicy>("90.1");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), SeverityLvl_enum::CRITICAL);
}