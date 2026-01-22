// tests/phase3/LogSinkFactoryTest.cpp

#include <gtest/gtest.h>
#include "inc/Formatter/LogSinkFactory.hpp"
#include "inc/Formatter/Enums.hpp"

using namespace logging;

class LogSinkFactoryTest : public ::testing::Test {};


TEST_F(LogSinkFactoryTest, CreateFileSink) {
    auto sink = LogSinkFactory::create(LogSinkType_enum::FILE, "test_output.log");
    
    ASSERT_NE(sink, nullptr);
}

TEST_F(LogSinkFactoryTest, CreateFileSinkWithDefaultConfig) {
    auto sink = LogSinkFactory::create(LogSinkType_enum::FILE);
    
    // Should create with default filename
    ASSERT_NE(sink, nullptr);
}

TEST_F(LogSinkFactoryTest, CreateMultipleSinks) {
    auto sink1 = LogSinkFactory::create(LogSinkType_enum::CONSOLE);
    auto sink2 = LogSinkFactory::create(LogSinkType_enum::CONSOLE);
    
    ASSERT_NE(sink1, nullptr);
    ASSERT_NE(sink2, nullptr);
    
    // Should be different instances
    EXPECT_NE(sink1.get(), sink2.get());
}

