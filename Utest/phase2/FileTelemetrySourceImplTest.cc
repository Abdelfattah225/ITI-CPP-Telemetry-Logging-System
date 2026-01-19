// Utest/phase2/FileTelemetrySourceImplTest.cc

#include <gtest/gtest.h>
#include "SmartDataHub/FileTelemetrySourceImpl.hpp"
#include <fstream>
#include <cstdio>

using namespace SmartDataHub;

class FileTelemetrySourceImplTest : public ::testing::Test
{
protected:
    const std::string testFilePath = "/tmp/telemetry_test.txt";
    const std::string testContent = "Line 1: Data\nLine 2: More Data\nLine 3: Final Data";

    void SetUp() override
    {
        std::ofstream ofs(testFilePath);
        ofs << testContent;
        ofs.close();
    }

    void TearDown() override
    {
        std::remove(testFilePath.c_str());
    }
};

// ══════════════════════════════════════════════════════════════════════
// Constructor Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(FileTelemetrySourceImplTest, Constructor_StoresPath)
{
    FileTelemetrySourceImpl source(testFilePath);
    // Path is stored internally - we can only verify through openSource
    EXPECT_TRUE(source.openSource());
}

// ══════════════════════════════════════════════════════════════════════
// openSource Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(FileTelemetrySourceImplTest, OpenSource_ValidPath_ReturnsTrue)
{
    FileTelemetrySourceImpl source(testFilePath);
    EXPECT_TRUE(source.openSource());
}

TEST_F(FileTelemetrySourceImplTest, OpenSource_InvalidPath_ReturnsFalse)
{
    FileTelemetrySourceImpl source("/nonexistent/path/file.txt");
    EXPECT_FALSE(source.openSource());
}

// ══════════════════════════════════════════════════════════════════════
// readSource Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(FileTelemetrySourceImplTest, ReadSource_ReadsLine)
{
    FileTelemetrySourceImpl source(testFilePath);
    source.openSource();

    std::string line;
    EXPECT_TRUE(source.readSource(line));
    EXPECT_EQ(line, "Line 1: Data");
}

TEST_F(FileTelemetrySourceImplTest, ReadSource_ReadsMultipleLines)
{
    FileTelemetrySourceImpl source(testFilePath);
    source.openSource();

    std::string line1, line2, line3;
    EXPECT_TRUE(source.readSource(line1));
    EXPECT_TRUE(source.readSource(line2));
    EXPECT_TRUE(source.readSource(line3));

    EXPECT_EQ(line1, "Line 1: Data");
    EXPECT_EQ(line2, "Line 2: More Data");
    EXPECT_EQ(line3, "Line 3: Final Data");
}

TEST_F(FileTelemetrySourceImplTest, ReadSource_NotOpened_ReturnsFalse)
{
    FileTelemetrySourceImpl source(testFilePath);
    // Don't call openSource()

    std::string line;
    EXPECT_FALSE(source.readSource(line));
}

// ══════════════════════════════════════════════════════════════════════
// Interface Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(FileTelemetrySourceImplTest, ImplementsITelemetrySource)
{
    ITelemetrySource* source = new FileTelemetrySourceImpl(testFilePath);
    
    EXPECT_TRUE(source->openSource());
    
    std::string line;
    EXPECT_TRUE(source->readSource(line));
    EXPECT_EQ(line, "Line 1: Data");
    
    delete source;
}

TEST_F(FileTelemetrySourceImplTest, PolymorphicUsage)
{
    std::unique_ptr<ITelemetrySource> source = 
        std::make_unique<FileTelemetrySourceImpl>(testFilePath);
    
    EXPECT_TRUE(source->openSource());
    
    std::string line;
    EXPECT_TRUE(source->readSource(line));
}

// ══════════════════════════════════════════════════════════════════════
// /proc File Tests (Real System Files)
// ══════════════════════════════════════════════════════════════════════

TEST_F(FileTelemetrySourceImplTest, ReadProcStat)
{
    FileTelemetrySourceImpl source("/proc/stat");
    EXPECT_TRUE(source.openSource());

    std::string line;
    EXPECT_TRUE(source.readSource(line));
    
    // /proc/stat first line starts with "cpu"
    EXPECT_TRUE(line.find("cpu") == 0);
}

TEST_F(FileTelemetrySourceImplTest, ReadProcMeminfo)
{
    FileTelemetrySourceImpl source("/proc/meminfo");
    EXPECT_TRUE(source.openSource());

    std::string line;
    EXPECT_TRUE(source.readSource(line));
    
    // /proc/meminfo first line starts with "MemTotal"
    EXPECT_TRUE(line.find("MemTotal") == 0);
}

// ══════════════════════════════════════════════════════════════════════
// Rule of 0 Tests (Move via RAII members)
// ══════════════════════════════════════════════════════════════════════

TEST_F(FileTelemetrySourceImplTest, MoveConstructor_Works)
{
    FileTelemetrySourceImpl source1(testFilePath);
    source1.openSource();

    FileTelemetrySourceImpl source2(std::move(source1));

    std::string line;
    EXPECT_TRUE(source2.readSource(line));
    EXPECT_EQ(line, "Line 1: Data");
}

TEST_F(FileTelemetrySourceImplTest, MoveAssignment_Works)
{
    FileTelemetrySourceImpl source1(testFilePath);
    source1.openSource();

    FileTelemetrySourceImpl source2("/tmp/dummy.txt");
    source2 = std::move(source1);

    std::string line;
    EXPECT_TRUE(source2.readSource(line));
    EXPECT_EQ(line, "Line 1: Data");
}