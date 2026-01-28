#include <gtest/gtest.h>
#include "inc/AsyncLogging/RingBuffer.hpp"
#include <string>

namespace async_logging
{
namespace test
{

// ============== Constructor Tests ==============

TEST(RingBufferTest, ConstructorInitializesEmpty)
{
    RingBuffer<int> buffer(5);
    
    EXPECT_TRUE(buffer.isEmpty());
    EXPECT_FALSE(buffer.isFull());
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.capacity(), 5);
}

TEST(RingBufferTest, ConstructorWithZeroCapacity)
{
    RingBuffer<int> buffer(0);
    
    EXPECT_TRUE(buffer.isEmpty());
    EXPECT_TRUE(buffer.isFull());
    EXPECT_EQ(buffer.capacity(), 0);
}

// ============== Push Tests ==============

TEST(RingBufferTest, TryPushSingleElement)
{
    RingBuffer<int> buffer(5);
    
    EXPECT_TRUE(buffer.tryPush(42));
    EXPECT_EQ(buffer.size(), 1);
    EXPECT_FALSE(buffer.isEmpty());
}

TEST(RingBufferTest, TryPushMultipleElements)
{
    RingBuffer<int> buffer(5);
    
    for (int i = 1; i <= 5; ++i)
    {
        EXPECT_TRUE(buffer.tryPush(i));
        EXPECT_EQ(buffer.size(), static_cast<std::size_t>(i));
    }
    
    EXPECT_TRUE(buffer.isFull());
}

TEST(RingBufferTest, TryPushWhenFull)
{
    RingBuffer<int> buffer(3);
    
    EXPECT_TRUE(buffer.tryPush(1));
    EXPECT_TRUE(buffer.tryPush(2));
    EXPECT_TRUE(buffer.tryPush(3));
    EXPECT_FALSE(buffer.tryPush(4));  // Should fail
    
    EXPECT_EQ(buffer.size(), 3);
}

// ============== Pop Tests ==============

TEST(RingBufferTest, TryPopSingleElement)
{
    RingBuffer<int> buffer(5);
    buffer.tryPush(42);
    
    auto result = buffer.tryPop();
    
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 42);
    EXPECT_TRUE(buffer.isEmpty());
}

TEST(RingBufferTest, TryPopMultipleElements)
{
    RingBuffer<int> buffer(5);
    
    for (int i = 1; i <= 5; ++i)
    {
        buffer.tryPush(i);
    }
    
    for (int i = 1; i <= 5; ++i)
    {
        auto result = buffer.tryPop();
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), i);
    }
    
    EXPECT_TRUE(buffer.isEmpty());
}

TEST(RingBufferTest, TryPopWhenEmpty)
{
    RingBuffer<int> buffer(5);
    
    auto result = buffer.tryPop();
    
    EXPECT_FALSE(result.has_value());
}

// ============== FIFO Order Tests ==============

TEST(RingBufferTest, FIFOOrder)
{
    RingBuffer<std::string> buffer(3);
    
    buffer.tryPush("first");
    buffer.tryPush("second");
    buffer.tryPush("third");
    
    EXPECT_EQ(buffer.tryPop().value(), "first");
    EXPECT_EQ(buffer.tryPop().value(), "second");
    EXPECT_EQ(buffer.tryPop().value(), "third");
}

// ============== Wrap Around Tests ==============

TEST(RingBufferTest, WrapAroundBehavior)
{
    RingBuffer<int> buffer(3);
    
    // Fill buffer
    buffer.tryPush(1);
    buffer.tryPush(2);
    buffer.tryPush(3);
    
    // Pop two elements
    EXPECT_EQ(buffer.tryPop().value(), 1);
    EXPECT_EQ(buffer.tryPop().value(), 2);
    
    // Push two more (should wrap around)
    buffer.tryPush(4);
    buffer.tryPush(5);
    
    // Verify order
    EXPECT_EQ(buffer.tryPop().value(), 3);
    EXPECT_EQ(buffer.tryPop().value(), 4);
    EXPECT_EQ(buffer.tryPop().value(), 5);
    EXPECT_TRUE(buffer.isEmpty());
}

TEST(RingBufferTest, MultipleWrapArounds)
{
    RingBuffer<int> buffer(2);
    
    for (int round = 0; round < 5; ++round)
    {
        EXPECT_TRUE(buffer.tryPush(round * 2));
        EXPECT_TRUE(buffer.tryPush(round * 2 + 1));
        
        EXPECT_EQ(buffer.tryPop().value(), round * 2);
        EXPECT_EQ(buffer.tryPop().value(), round * 2 + 1);
    }
}

// ============== Move Semantics Tests ==============

TEST(RingBufferTest, MoveOnlyType)
{
    RingBuffer<std::unique_ptr<int>> buffer(3);
    
    buffer.tryPush(std::make_unique<int>(42));
    
    auto result = buffer.tryPop();
    
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result.value(), 42);
}

TEST(RingBufferTest, MoveConstructor)
{
    RingBuffer<int> buffer1(3);
    buffer1.tryPush(1);
    buffer1.tryPush(2);
    
    RingBuffer<int> buffer2(std::move(buffer1));
    
    EXPECT_EQ(buffer2.size(), 2);
    EXPECT_EQ(buffer2.tryPop().value(), 1);
    EXPECT_EQ(buffer2.tryPop().value(), 2);
}

TEST(RingBufferTest, MoveAssignment)
{
    RingBuffer<int> buffer1(3);
    buffer1.tryPush(1);
    buffer1.tryPush(2);
    
    RingBuffer<int> buffer2(5);
    buffer2 = std::move(buffer1);
    
    EXPECT_EQ(buffer2.size(), 2);
    EXPECT_EQ(buffer2.tryPop().value(), 1);
}

// ============== Utility Method Tests ==============

TEST(RingBufferTest, IsEmptyAfterPushPop)
{
    RingBuffer<int> buffer(3);
    
    EXPECT_TRUE(buffer.isEmpty());
    
    buffer.tryPush(1);
    EXPECT_FALSE(buffer.isEmpty());
    
    buffer.tryPop();
    EXPECT_TRUE(buffer.isEmpty());
}

TEST(RingBufferTest, IsFullAfterFilling)
{
    RingBuffer<int> buffer(3);
    
    EXPECT_FALSE(buffer.isFull());
    
    buffer.tryPush(1);
    buffer.tryPush(2);
    EXPECT_FALSE(buffer.isFull());
    
    buffer.tryPush(3);
    EXPECT_TRUE(buffer.isFull());
    
    buffer.tryPop();
    EXPECT_FALSE(buffer.isFull());
}

TEST(RingBufferTest, SizeTracking)
{
    RingBuffer<int> buffer(5);
    
    EXPECT_EQ(buffer.size(), 0);
    
    buffer.tryPush(1);
    EXPECT_EQ(buffer.size(), 1);
    
    buffer.tryPush(2);
    buffer.tryPush(3);
    EXPECT_EQ(buffer.size(), 3);
    
    buffer.tryPop();
    EXPECT_EQ(buffer.size(), 2);
    
    buffer.tryPop();
    buffer.tryPop();
    EXPECT_EQ(buffer.size(), 0);
}

} // namespace test
} // namespace async_logging