// Utest/phase2/SafeSocketTest.cc

#include <gtest/gtest.h>
#include "SmartDataHub/SafeSocket.hpp"
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>

using namespace SmartDataHub;

class SafeSocketTest : public ::testing::Test
{
protected:
    const std::string testSocketPath = "/tmp/safe_socket_test.sock";

    void SetUp() override
    {
        // Remove socket file if exists
        std::remove(testSocketPath.c_str());
    }

    void TearDown() override
    {
        std::remove(testSocketPath.c_str());
    }

    // Helper to create a simple server
    void startTestServer(std::function<void(int)> handler)
    {
        serverThread = std::thread([this, handler]() {
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

            int clientFd = accept(serverFd, nullptr, nullptr);
            if (clientFd >= 0)
            {
                handler(clientFd);
                close(clientFd);
            }

            close(serverFd);
        });

        // Wait for server to be ready
        while (!serverReady)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    void stopTestServer()
    {
        if (serverThread.joinable())
        {
            serverThread.join();
        }
    }

private:
    std::thread serverThread;
    std::atomic<bool> serverReady{false};
};

// ══════════════════════════════════════════════════════════════════════
// Constructor Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SafeSocketTest, DefaultConstructor_CreatesInvalidSocket)
{
    SafeSocket socket;
    EXPECT_FALSE(socket.isConnected());
    EXPECT_EQ(socket.getFd(), -1);
}

// ══════════════════════════════════════════════════════════════════════
// Create Socket Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SafeSocketTest, CreateSocket_ReturnsTrue)
{
    SafeSocket socket;
    EXPECT_TRUE(socket.createSocket());
    EXPECT_TRUE(socket.isConnected());
    EXPECT_GE(socket.getFd(), 0);
}

// ══════════════════════════════════════════════════════════════════════
// Connect Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SafeSocketTest, ConnectSocket_NoServer_ReturnsFalse)
{
    SafeSocket socket;
    socket.createSocket();
    EXPECT_FALSE(socket.connectSocket("/tmp/nonexistent.sock"));
}

TEST_F(SafeSocketTest, ConnectSocket_WithServer_ReturnsTrue)
{
    startTestServer([](int clientFd) {
        // Simple echo server
        char buf[1024];
        ssize_t n = recv(clientFd, buf, sizeof(buf), 0);
        if (n > 0)
        {
            send(clientFd, buf, n, 0);
        }
    });

    SafeSocket socket;
    EXPECT_TRUE(socket.createSocket());
    EXPECT_TRUE(socket.connectSocket(testSocketPath));

    stopTestServer();
}

// ══════════════════════════════════════════════════════════════════════
// Send/Receive Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SafeSocketTest, SendAndReceiveData)
{
    startTestServer([](int clientFd) {
        char buf[1024];
        ssize_t n = recv(clientFd, buf, sizeof(buf), 0);
        if (n > 0)
        {
            send(clientFd, buf, n, 0);
        }
    });

    SafeSocket socket;
    socket.createSocket();
    socket.connectSocket(testSocketPath);

    std::string sendMsg = "Hello, Server!";
    ssize_t sent = socket.sendData(sendMsg);
    EXPECT_EQ(sent, static_cast<ssize_t>(sendMsg.size()));

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    std::string recvMsg;
    ssize_t received = socket.receiveData(recvMsg);
    EXPECT_GT(received, 0);
    EXPECT_EQ(recvMsg, sendMsg);

    stopTestServer();
}

// ══════════════════════════════════════════════════════════════════════
// Close Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SafeSocketTest, CloseSocket_ClosesConnection)
{
    SafeSocket socket;
    socket.createSocket();
    EXPECT_TRUE(socket.isConnected());

    socket.closeSocket();
    EXPECT_FALSE(socket.isConnected());
    EXPECT_EQ(socket.getFd(), -1);
}

TEST_F(SafeSocketTest, CloseSocket_CalledTwice_NoError)
{
    SafeSocket socket;
    socket.createSocket();
    socket.closeSocket();
    socket.closeSocket();  // Should not crash
    EXPECT_FALSE(socket.isConnected());
}

// ══════════════════════════════════════════════════════════════════════
// Move Semantics Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SafeSocketTest, MoveConstructor_TransfersOwnership)
{
    SafeSocket socket1;
    socket1.createSocket();
    int originalFd = socket1.getFd();

    SafeSocket socket2(std::move(socket1));

    EXPECT_FALSE(socket1.isConnected());
    EXPECT_EQ(socket1.getFd(), -1);

    EXPECT_TRUE(socket2.isConnected());
    EXPECT_EQ(socket2.getFd(), originalFd);
}

TEST_F(SafeSocketTest, MoveAssignment_TransfersOwnership)
{
    SafeSocket socket1;
    socket1.createSocket();
    int originalFd = socket1.getFd();

    SafeSocket socket2;
    socket2 = std::move(socket1);

    EXPECT_FALSE(socket1.isConnected());
    EXPECT_EQ(socket1.getFd(), -1);

    EXPECT_TRUE(socket2.isConnected());
    EXPECT_EQ(socket2.getFd(), originalFd);
}

TEST_F(SafeSocketTest, SelfMoveAssignment_NoChange)
{
    SafeSocket socket;
    socket.createSocket();
    int originalFd = socket.getFd();

    socket = std::move(socket);

    EXPECT_TRUE(socket.isConnected());
    EXPECT_EQ(socket.getFd(), originalFd);
}

// ══════════════════════════════════════════════════════════════════════
// RAII Tests
// ══════════════════════════════════════════════════════════════════════

TEST_F(SafeSocketTest, Destructor_ClosesSocket)
{
    {
        SafeSocket socket;
        socket.createSocket();
        EXPECT_TRUE(socket.isConnected());
    }
    // Socket should be closed when going out of scope - no crash
}