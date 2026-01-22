// tests/phase3/PoliciesTest.cpp

#include <gtest/gtest.h>
#include "inc/Formatter/Policies.hpp"
#include "inc/Formatter/Enums.hpp"

using namespace logging;

// ============== CpuPolicy Tests ==============

class CpuPolicyTest : public ::testing::Test {};

TEST_F(CpuPolicyTest, HasCorrectContext) {
    EXPECT_EQ(CpuPolicy::context, TelemetrySrc_enum::CPU);
}

TEST_F(CpuPolicyTest, HasCorrectUnit) {
    EXPECT_EQ(CpuPolicy::unit, "%");
}

TEST_F(CpuPolicyTest, HasCorrectThresholds) {
    EXPECT_FLOAT_EQ(CpuPolicy::WARNING, 75.0f);
    EXPECT_FLOAT_EQ(CpuPolicy::CRITICAL, 90.0f);
}

TEST_F(CpuPolicyTest, InferSeverityInfo) {
    EXPECT_EQ(CpuPolicy::inferSeverity(0.0f), SeverityLvl_enum::INFO);
    EXPECT_EQ(CpuPolicy::inferSeverity(50.0f), SeverityLvl_enum::INFO);
    EXPECT_EQ(CpuPolicy::inferSeverity(74.9f), SeverityLvl_enum::INFO);
    EXPECT_EQ(CpuPolicy::inferSeverity(75.0f), SeverityLvl_enum::INFO);  // Boundary
}

TEST_F(CpuPolicyTest, InferSeverityWarning) {
    EXPECT_EQ(CpuPolicy::inferSeverity(75.1f), SeverityLvl_enum::WARNING);
    EXPECT_EQ(CpuPolicy::inferSeverity(80.0f), SeverityLvl_enum::WARNING);
    EXPECT_EQ(CpuPolicy::inferSeverity(89.9f), SeverityLvl_enum::WARNING);
    EXPECT_EQ(CpuPolicy::inferSeverity(90.0f), SeverityLvl_enum::WARNING);  // Boundary
}

TEST_F(CpuPolicyTest, InferSeverityCritical) {
    EXPECT_EQ(CpuPolicy::inferSeverity(90.1f), SeverityLvl_enum::CRITICAL);
    EXPECT_EQ(CpuPolicy::inferSeverity(95.0f), SeverityLvl_enum::CRITICAL);
    EXPECT_EQ(CpuPolicy::inferSeverity(100.0f), SeverityLvl_enum::CRITICAL);
}

TEST_F(CpuPolicyTest, InferSeverityIsConstexpr) {
    // Compile-time evaluation test
    constexpr auto severity = CpuPolicy::inferSeverity(85.0f);
    static_assert(severity == SeverityLvl_enum::WARNING, "Should be WARNING");
    EXPECT_EQ(severity, SeverityLvl_enum::WARNING);
}

// ============== GpuPolicy Tests ==============

class GpuPolicyTest : public ::testing::Test {};

TEST_F(GpuPolicyTest, HasCorrectContext) {
    EXPECT_EQ(GpuPolicy::context, TelemetrySrc_enum::GPU);
}

TEST_F(GpuPolicyTest, HasCorrectThresholds) {
    EXPECT_FLOAT_EQ(GpuPolicy::WARNING, 80.0f);
    EXPECT_FLOAT_EQ(GpuPolicy::CRITICAL, 95.0f);
}

TEST_F(GpuPolicyTest, InferSeverityInfo) {
    EXPECT_EQ(GpuPolicy::inferSeverity(50.0f), SeverityLvl_enum::INFO);
    EXPECT_EQ(GpuPolicy::inferSeverity(80.0f), SeverityLvl_enum::INFO);
}

TEST_F(GpuPolicyTest, InferSeverityWarning) {
    EXPECT_EQ(GpuPolicy::inferSeverity(80.1f), SeverityLvl_enum::WARNING);
    EXPECT_EQ(GpuPolicy::inferSeverity(90.0f), SeverityLvl_enum::WARNING);
}

TEST_F(GpuPolicyTest, InferSeverityCritical) {
    EXPECT_EQ(GpuPolicy::inferSeverity(95.1f), SeverityLvl_enum::CRITICAL);
    EXPECT_EQ(GpuPolicy::inferSeverity(100.0f), SeverityLvl_enum::CRITICAL);
}

// ============== RamPolicy Tests ==============

class RamPolicyTest : public ::testing::Test {};

TEST_F(RamPolicyTest, HasCorrectContext) {
    EXPECT_EQ(RamPolicy::context, TelemetrySrc_enum::RAM);
}

TEST_F(RamPolicyTest, HasCorrectThresholds) {
    EXPECT_FLOAT_EQ(RamPolicy::WARNING, 70.0f);
    EXPECT_FLOAT_EQ(RamPolicy::CRITICAL, 85.0f);
}

TEST_F(RamPolicyTest, InferSeverityInfo) {
    EXPECT_EQ(RamPolicy::inferSeverity(50.0f), SeverityLvl_enum::INFO);
    EXPECT_EQ(RamPolicy::inferSeverity(70.0f), SeverityLvl_enum::INFO);
}

TEST_F(RamPolicyTest, InferSeverityWarning) {
    EXPECT_EQ(RamPolicy::inferSeverity(70.1f), SeverityLvl_enum::WARNING);
    EXPECT_EQ(RamPolicy::inferSeverity(85.0f), SeverityLvl_enum::WARNING);
}

TEST_F(RamPolicyTest, InferSeverityCritical) {
    EXPECT_EQ(RamPolicy::inferSeverity(85.1f), SeverityLvl_enum::CRITICAL);
    EXPECT_EQ(RamPolicy::inferSeverity(100.0f), SeverityLvl_enum::CRITICAL);
}

// ============== Policy Validation Tests ==============

class PolicyValidationTest : public ::testing::Test {};

TEST_F(PolicyValidationTest, AllPoliciesAreValid) {
    EXPECT_TRUE(isValidPolicy<CpuPolicy>());
    EXPECT_TRUE(isValidPolicy<GpuPolicy>());
    EXPECT_TRUE(isValidPolicy<RamPolicy>());
}

TEST_F(PolicyValidationTest, WarningLessThanCritical) {
    EXPECT_LT(CpuPolicy::WARNING, CpuPolicy::CRITICAL);
    EXPECT_LT(GpuPolicy::WARNING, GpuPolicy::CRITICAL);
    EXPECT_LT(RamPolicy::WARNING, RamPolicy::CRITICAL);
}