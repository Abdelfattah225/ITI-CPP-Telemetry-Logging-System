// Utest/phase2/SocketTelemetrySourceImplTest.cc

#include <gtest/gtest.h>
#include "SmartDataHub/SocketTelemetrySourceImpl.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>

using namespace SmartDataHub;

class SocketTelemetrySourceImplTest : public ::testing::Test
{
protected:
    const std::string testSocketPath = "/tmp/telemetry_socket_test.sock";
    std::thread serverThread;
    std::atomic<bool> serverReady{false};
    std::atomic<bool> stopServer{false};

    void SetUp() override
    {
        std::remove(testSocketPath.c_str());
        serverReady = false;
        stopServer = false;
    }

    void TearDown() override
    {
        stopServer = true;
        if (serverThread.joinable())
        {
            serverThread.join();
        }
        std::remove(testSocketPath.c_str());
    }

    void startTestServer(const std::string& responseData)
    {
        serverThread = std::thread([this, responseData]() {
            int serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
            if (serverFd < 0) return;

            sockaddr_un addr;
            memset(&addr, 0, sizeof(addr));
            addr.sun_family = AF_UNIX;
            strncpy(addr.sun_path, testSocketPath.c_str(), sizeof(addr.sun_path) - 1);

            if (bind(serverFd, (sockaddr*)&addr, sizeof(addr)) < 0)
            {
                close(serverFd);
                return;
            }

            listen(serverFd, 1);
            serverReady = true;

            // Set timeout for accept
            struct timeval tv;
            tv.tv_sec = 2;
            tv.tv_usec = 0;
            setsockopt(serverFd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

            int clientFd = accept(serverFd, nullptr, nullptr);
            if (clientFd >= 0)
            {
                send(clientFd, responseData.c_str(), responseData.size(), 0);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                close(clientFd);
            }

            close(serverFd);
        });

        // Wait for server to be ready
        while (!serverReady && !stopServer)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
};

// ══════════════════════════════════════════════════════════════════════
// Constructor Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SocketTelemetrySourceImplTest, Constructor_StoresPath)
{
    SocketTelemetrySourceImpl source(testSocketPath);
    // Path is stored internally
}

// ══════════════════════════════════════════════════════════════════════
// openSource Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SocketTelemetrySourceImplTest, OpenSource_NoServer_ReturnsFalse)
{
    SocketTelemetrySourceImpl source(testSocketPath);
    EXPECT_FALSE(source.openSource());
}

TEST_F(SocketTelemetrySourceImplTest, OpenSource_WithServer_ReturnsTrue)
{
    startTestServer("Test Data");

    SocketTelemetrySourceImpl source(testSocketPath);
    EXPECT_TRUE(source.openSource());
}

// ══════════════════════════════════════════════════════════════════════
// readSource Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SocketTelemetrySourceImplTest, ReadSource_ReceivesData)
{
    const std::string testData = "CPU: 45.5%";
    startTestServer(testData);

    SocketTelemetrySourceImpl source(testSocketPath);
    source.openSource();

    std::string data;
    EXPECT_TRUE(source.readSource(data));
    EXPECT_EQ(data, testData);
}

TEST_F(SocketTelemetrySourceImplTest, ReadSource_NotOpened_ReturnsFalse)
{
    SocketTelemetrySourceImpl source(testSocketPath);
    // Don't call openSource()

    std::string data;
    EXPECT_FALSE(source.readSource(data));
}

// ══════════════════════════════════════════════════════════════════════
// Interface Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SocketTelemetrySourceImplTest, ImplementsITelemetrySource)
{
    startTestServer("Interface Test Data");

    ITelemetrySource* source = new SocketTelemetrySourceImpl(testSocketPath);
    
    EXPECT_TRUE(source->openSource());
    
    std::string data;
    EXPECT_TRUE(source->readSource(data));
    EXPECT_EQ(data, "Interface Test Data");
    
    delete source;
}

TEST_F(SocketTelemetrySourceImplTest, PolymorphicUsage)
{
    startTestServer("Polymorphic Data");

    std::unique_ptr<ITelemetrySource> source = 
        std::make_unique<SocketTelemetrySourceImpl>(testSocketPath);
    
    EXPECT_TRUE(source->openSource());
    
    std::string data;
    EXPECT_TRUE(source->readSource(data));
    EXPECT_EQ(data, "Polymorphic Data");
}

// ══════════════════════════════════════════════════════════════════════
// Rule of 0 Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SocketTelemetrySourceImplTest, MoveConstructor_Works)
{
    startTestServer("Move Constructor Data");

    SocketTelemetrySourceImpl source1(testSocketPath);
    source1.openSource();

    SocketTelemetrySourceImpl source2(std::move(source1));

    std::string data;
    EXPECT_TRUE(source2.readSource(data));
    EXPECT_EQ(data, "Move Constructor Data");
}