// tests/phase3/LogManagerBuilderTest.cpp

#include <gtest/gtest.h>
#include "inc/Formatter/LogManagerBuilder.hpp"
#include "inc/Formatter/Enums.hpp"
#include "inc/logging/LogMessage.hpp"

using namespace logging;

class LogManagerBuilderTest : public ::testing::Test {};

TEST_F(LogManagerBuilderTest, BuildWithDefaults) {
    auto manager = LogManagerBuilder().build();
    
    ASSERT_NE(manager, nullptr);
}

TEST_F(LogManagerBuilderTest, BuildWithAppName) {
    auto manager = LogManagerBuilder()
        .setAppName("TestApp")
        .build();
    
    ASSERT_NE(manager, nullptr);
}

TEST_F(LogManagerBuilderTest, BuildWithConsoleSink) {
    auto manager = LogManagerBuilder()
        .setAppName("TestApp")
        .addSink(LogSinkType_enum::CONSOLE)
        .build();
    
    ASSERT_NE(manager, nullptr);
}

TEST_F(LogManagerBuilderTest, BuildWithMultipleSinks) {
    auto manager = LogManagerBuilder()
        .setAppName("TestApp")
        .addSink(LogSinkType_enum::CONSOLE)
        .addSink(LogSinkType_enum::FILE, "test.log")
        .build();
    
    ASSERT_NE(manager, nullptr);
}

TEST_F(LogManagerBuilderTest, BuildWithBufferSize) {
    auto manager = LogManagerBuilder()
        .setAppName("TestApp")
        .setBufferSize(100)
        .build();
    
    ASSERT_NE(manager, nullptr);
}

TEST_F(LogManagerBuilderTest, MethodChaining) {
    // Verify fluent interface works
    auto manager = LogManagerBuilder()
        .setAppName("TestApp")
        .addSink(LogSinkType_enum::CONSOLE)
        .addSink(LogSinkType_enum::FILE, "app.log")
        .setBufferSize(50)
        .build();
    
    ASSERT_NE(manager, nullptr);
}

TEST_F(LogManagerBuilderTest, BuilderReturnsReference) {
    LogManagerBuilder builder;
    
    // Each method should return reference to builder
    auto& ref1 = builder.setAppName("App");
    auto& ref2 = ref1.addSink(LogSinkType_enum::CONSOLE);
    auto& ref3 = ref2.setBufferSize(10);
    
    // All should point to same builder
    EXPECT_EQ(&ref1, &builder);
    EXPECT_EQ(&ref2, &builder);
    EXPECT_EQ(&ref3, &builder);
}

TEST_F(LogManagerBuilderTest, IntegrationWithLogMessage) {
    auto manager = LogManagerBuilder()
        .setAppName("IntegrationTest")
        .addSink(LogSinkType_enum::CONSOLE)
        .build();
    
    ASSERT_NE(manager, nullptr);
    
    // Create a log message and log it
    LogMessage msg("IntegrationTest", Context::CPU, 75);
    
    EXPECT_NO_THROW(manager->log(msg));
}