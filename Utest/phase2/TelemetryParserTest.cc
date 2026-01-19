// Utest/phase2/TelemetryParserTest.cc

#include <gtest/gtest.h>
#include "SmartDataHub/TelemetryParser.hpp"
#include <thread>
#include <chrono>

using namespace SmartDataHub;

class TelemetryParserTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

// ══════════════════════════════════════════════════════════════════════
// Constructor Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(TelemetryParserTest, DefaultConstructor)
{
    TelemetryParser parser;
    // Parser should be constructed without errors
}

// ══════════════════════════════════════════════════════════════════════
// CPU Usage Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(TelemetryParserTest, GetCpuUsage_ReturnsValidPercentage)
{
    TelemetryParser parser;
    
    // First call initializes
    double usage1 = parser.getCpuUsage();
    EXPECT_GE(usage1, 0.0);
    
    // Wait a bit for CPU activity
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Second call should return valid percentage
    double usage2 = parser.getCpuUsage();
    EXPECT_GE(usage2, 0.0);
    EXPECT_LE(usage2, 100.0);
}

TEST_F(TelemetryParserTest, GetCpuString_ReturnsFormattedString)
{
    TelemetryParser parser;
    
    // First call
    std::string cpuStr = parser.getCpuString();
    EXPECT_TRUE(cpuStr.find("CPU:") != std::string::npos);
    
    // Wait and second call
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    cpuStr = parser.getCpuString();
    EXPECT_TRUE(cpuStr.find("CPU:") != std::string::npos);
    EXPECT_TRUE(cpuStr.find("%") != std::string::npos);
}

// ══════════════════════════════════════════════════════════════════════
// Memory Usage Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(TelemetryParserTest, GetMemUsage_ReturnsValidPercentage)
{
    TelemetryParser parser;
    
    double usage = parser.getMemUsage();
    
    // Memory usage should be between 0 and 100
    EXPECT_GE(usage, 0.0);
    EXPECT_LE(usage, 100.0);
    
    // Should be positive (some memory is always used)
    EXPECT_GT(usage, 0.0);
}

TEST_F(TelemetryParserTest, GetMemString_ReturnsFormattedString)
{
    TelemetryParser parser;
    
    std::string memStr = parser.getMemString();
    
    EXPECT_TRUE(memStr.find("Memory:") != std::string::npos);
    EXPECT_TRUE(memStr.find("%") != std::string::npos);
}

// ══════════════════════════════════════════════════════════════════════
// Multiple Reads Test
// ══════════════════════════════════════════════════════════════════════

TEST_F(TelemetryParserTest, MultipleReads_ReturnValidData)
{
    TelemetryParser parser;
    
    for (int i = 0; i < 5; ++i)
    {
        double cpu = parser.getCpuUsage();
        double mem = parser.getMemUsage();
        
        EXPECT_GE(cpu, 0.0);
        EXPECT_LE(cpu, 100.0);
        EXPECT_GE(mem, 0.0);
        EXPECT_LE(mem, 100.0);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// ══════════════════════════════════════════════════════════════════════
// Output Format Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(TelemetryParserTest, CpuString_Format)
{
    TelemetryParser parser;
    
    // Initialize
    parser.getCpuUsage();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::string cpuStr = parser.getCpuString();
    
    // Should match format: "CPU: XX.X%"
    EXPECT_TRUE(cpuStr.substr(0, 4) == "CPU:");
}

TEST_F(TelemetryParserTest, MemString_Format)
{
    TelemetryParser parser;
    
    std::string memStr = parser.getMemString();
    
    // Should match format: "Memory: XX.X%"
    EXPECT_TRUE(memStr.substr(0, 7) == "Memory:");
}

// ══════════════════════════════════════════════════════════════════════
// Edge Cases
// ══════════════════════════════════════════════════════════════════════

TEST_F(TelemetryParserTest, RapidSuccessiveCalls)
{
    TelemetryParser parser;
    
    // Rapid successive calls should not crash
    for (int i = 0; i < 10; ++i)
    {
        parser.getCpuUsage();
        parser.getMemUsage();
    }
}

// ══════════════════════════════════════════════════════════════════════
// Rule of 0 Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(TelemetryParserTest, MoveConstructor_Works)
{
    TelemetryParser parser1;
    parser1.getCpuUsage();  // Initialize
    
    TelemetryParser parser2(std::move(parser1));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    double cpu = parser2.getCpuUsage();
    EXPECT_GE(cpu, 0.0);
    EXPECT_LE(cpu, 100.0);
}

TEST_F(TelemetryParserTest, MoveAssignment_Works)
{
    TelemetryParser parser1;
    parser1.getCpuUsage();
    
    TelemetryParser parser2;
    parser2 = std::move(parser1);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    double mem = parser2.getMemUsage();
    EXPECT_GE(mem, 0.0);
    EXPECT_LE(mem, 100.0);
}