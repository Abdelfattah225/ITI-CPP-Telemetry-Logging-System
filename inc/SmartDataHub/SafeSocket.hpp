#pragma once
#include <sys/socket.h> // socket(), connect(), send(), recv()
#include <sys/un.h>     // sockaddr_un (Unix socket address structure)
#include <unistd.h>     // close()
#include <string>
#include <cstring> // memset(), strcpy()

namespace SmartDataHub
{
    class SafeSocket
    {
    private:
        int sockfd = -1 ; 
    public:
        // Constructors
        SafeSocket() = default;

        // Rule of 5
        ~SafeSocket();
        SafeSocket(SafeSocket &&other) noexcept;
        SafeSocket &operator=(SafeSocket &&other) noexcept;
        SafeSocket(const SafeSocket &) = delete;
        SafeSocket &operator=(const SafeSocket &) = delete;

        // Operations
        bool createSocket();                         // Creates socket
        bool connectSocket(const std::string &path); // Connects to server
        ssize_t sendData(const std::string &data);   // Sends data
        ssize_t receiveData(std::string &data);      // Receives data
        void closeSocket();                          // Closes socket

        // Utilities
        bool isConnected() const;
        int getFd() const;
    };

} // namespace SmartDataHub
