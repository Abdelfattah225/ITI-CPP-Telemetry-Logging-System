// Utest/phase2/SafeFileTest.cc

#include <gtest/gtest.h>
#include "SmartDataHub/SafeFile.hpp"
#include <fstream>
#include <cstdio>
#include <fcntl.h>

using namespace SmartDataHub;

class SafeFileTest : public ::testing::Test
{
protected:
    const std::string testFilePath = "/tmp/safe_file_test.txt";
    const std::string testContent = "Hello, SafeFile!\nLine 2\nLine 3";

    void SetUp() override
    {
        // Create test file
        std::ofstream ofs(testFilePath);
        ofs << testContent;
        ofs.close();
    }

    void TearDown() override
    {
        // Remove test file
        std::remove(testFilePath.c_str());
    }
};

// ══════════════════════════════════════════════════════════════════════
// Constructor Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SafeFileTest, DefaultConstructor_CreatesInvalidFile)
{
    SafeFile file;
    EXPECT_FALSE(file.isOpen());
    EXPECT_EQ(file.getFd(), -1);
}

TEST_F(SafeFileTest, ParameterizedConstructor_OpensFile)
{
    SafeFile file(testFilePath);
    EXPECT_TRUE(file.isOpen());
    EXPECT_GE(file.getFd(), 0);
}

// ══════════════════════════════════════════════════════════════════════
// Open/Close Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SafeFileTest, OpenFile_ValidPath_ReturnsTrue)
{
    SafeFile file;
    EXPECT_TRUE(file.openFile(testFilePath.c_str(), O_RDONLY));
    EXPECT_TRUE(file.isOpen());
}

TEST_F(SafeFileTest, OpenFile_InvalidPath_ReturnsFalse)
{
    SafeFile file;
    EXPECT_FALSE(file.openFile("/nonexistent/path/file.txt", O_RDONLY));
    EXPECT_FALSE(file.isOpen());
}

TEST_F(SafeFileTest, CloseFile_ClosesFileDescriptor)
{
    SafeFile file;
    file.openFile(testFilePath.c_str(), O_RDONLY);
    EXPECT_TRUE(file.isOpen());
    
    file.closeFile();
    EXPECT_FALSE(file.isOpen());
    EXPECT_EQ(file.getFd(), -1);
}

TEST_F(SafeFileTest, CloseFile_CalledTwice_NoError)
{
    SafeFile file;
    file.openFile(testFilePath.c_str(), O_RDONLY);
    file.closeFile();
    file.closeFile();  // Should not crash
    EXPECT_FALSE(file.isOpen());
}

// ══════════════════════════════════════════════════════════════════════
// Read Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SafeFileTest, Readline_ReadsFirstLine)
{
    SafeFile file;
    file.openFile(testFilePath.c_str(), O_RDONLY);
    
    std::string line;
    EXPECT_TRUE(file.readline(line));
    EXPECT_EQ(line, "Hello, SafeFile!");
}

TEST_F(SafeFileTest, Readline_ReadsMultipleLines)
{
    SafeFile file;
    file.openFile(testFilePath.c_str(), O_RDONLY);
    
    std::string line1, line2, line3;
    EXPECT_TRUE(file.readline(line1));
    EXPECT_TRUE(file.readline(line2));
    EXPECT_TRUE(file.readline(line3));
    
    EXPECT_EQ(line1, "Hello, SafeFile!");
    EXPECT_EQ(line2, "Line 2");
    EXPECT_EQ(line3, "Line 3");
}

TEST_F(SafeFileTest, Readline_FileNotOpen_ReturnsFalse)
{
    SafeFile file;
    std::string line;
    EXPECT_FALSE(file.readline(line));
}

// ══════════════════════════════════════════════════════════════════════
// Write Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SafeFileTest, Writeline_WritesToFile)
{
    const std::string writeTestPath = "/tmp/safe_file_write_test.txt";
    
    {
        SafeFile file;
        file.openFile(writeTestPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
        std::string content = "Test write content";
        EXPECT_TRUE(file.writeline(content));
    }
    
    // Verify content
    std::ifstream ifs(writeTestPath);
    std::string readContent;
    std::getline(ifs, readContent);
    EXPECT_EQ(readContent, "Test write content");
    
    std::remove(writeTestPath.c_str());
}

// ══════════════════════════════════════════════════════════════════════
// Move Semantics Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SafeFileTest, MoveConstructor_TransfersOwnership)
{
    SafeFile file1;
    file1.openFile(testFilePath.c_str(), O_RDONLY);
    int originalFd = file1.getFd();
    
    SafeFile file2(std::move(file1));
    
    // file1 should be invalidated
    EXPECT_FALSE(file1.isOpen());
    EXPECT_EQ(file1.getFd(), -1);
    
    // file2 should own the resource
    EXPECT_TRUE(file2.isOpen());
    EXPECT_EQ(file2.getFd(), originalFd);
}

TEST_F(SafeFileTest, MoveAssignment_TransfersOwnership)
{
    SafeFile file1;
    file1.openFile(testFilePath.c_str(), O_RDONLY);
    int originalFd = file1.getFd();
    
    SafeFile file2;
    file2 = std::move(file1);
    
    EXPECT_FALSE(file1.isOpen());
    EXPECT_EQ(file1.getFd(), -1);
    
    EXPECT_TRUE(file2.isOpen());
    EXPECT_EQ(file2.getFd(), originalFd);
}

TEST_F(SafeFileTest, MoveAssignment_ClosesExistingResource)
{
    const std::string testFile2 = "/tmp/safe_file_test2.txt";
    std::ofstream(testFile2) << "Test";
    
    SafeFile file1;
    file1.openFile(testFilePath.c_str(), O_RDONLY);
    
    SafeFile file2;
    file2.openFile(testFile2.c_str(), O_RDONLY);
    int file2OldFd = file2.getFd();
    
    file2 = std::move(file1);
    
    // file2's old fd should be closed, new fd should be file1's old fd
    EXPECT_NE(file2.getFd(), file2OldFd);
    
    std::remove(testFile2.c_str());
}

TEST_F(SafeFileTest, SelfMoveAssignment_NoChange)
{
    SafeFile file;
    file.openFile(testFilePath.c_str(), O_RDONLY);
    int originalFd = file.getFd();
    
    file = std::move(file);
    
    EXPECT_TRUE(file.isOpen());
    EXPECT_EQ(file.getFd(), originalFd);
}

// ══════════════════════════════════════════════════════════════════════
// RAII Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SafeFileTest, Destructor_ClosesFile)
{
    int fd;
    {
        SafeFile file;
        file.openFile(testFilePath.c_str(), O_RDONLY);
        fd = file.getFd();
        EXPECT_GE(fd, 0);
    }
    // File should be closed when file goes out of scope
    // We can't easily verify the fd is closed, but we can verify no crash
}

// ══════════════════════════════════════════════════════════════════════
// Copy Semantics (Should Not Compile - Just Documentation)
// ══════════════════════════════════════════════════════════════════════

// These tests verify copy is deleted (uncomment to verify compilation fails)
// TEST_F(SafeFileTest, CopyConstructor_Deleted)
// {
//     SafeFile file1;
//     SafeFile file2(file1);  // Should not compile
// }

// TEST_F(SafeFileTest, CopyAssignment_Deleted)
// {
//     SafeFile file1, file2;
//     file2 = file1;  // Should not compile
// }