#include <SafeSocket.hpp>

namespace SmartDataHub
{

    // Rule of 5
    SafeSocket::SafeSocket(SafeSocket &&other) noexcept : sockfd{other.sockfd}
    {
        other.sockfd = -1;   
    }
    SafeSocket& SafeSocket::operator=(SafeSocket &&other) noexcept 
    {
        if(this != &other){
            closeSocket();
            this->sockfd = other.sockfd;
            other.sockfd = -1;
        }
        return *this;
    }
    SafeSocket::~SafeSocket()
    {
        closeSocket();
    }

    // Operations
    bool SafeSocket::createSocket()
    {
        sockfd = socket(AF_UNIX, SOCK_NONBLOCK | SOCK_STREAM, 0);
        return sockfd >= 0;
    }

    bool SafeSocket::connectSocket(const std::string &path)
    {
        // setup address
        sockaddr_un addr;
        // remove garbage ==> clean up the structure
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);
        int result = connect(sockfd, (sockaddr *)&addr, sizeof(addr));
        return result == 0;
    } // Connects to server

    ssize_t SafeSocket::sendData(const std::string &data)
    {
        return send(sockfd, data.c_str(), data.size(), 0);
    } // Sends data
    ssize_t SafeSocket::receiveData(std::string &data)
    {
        char buffer[1024];
        ssize_t NoOfBytes = recv(sockfd, buffer, sizeof(buffer)-1, 0);
        if (NoOfBytes > 0)
        {
            buffer[NoOfBytes] = '\0';
            data = buffer;
        }
        return NoOfBytes;

    } // Receives data
    void SafeSocket::closeSocket()
    {
        if(sockfd >= 0){
            close(sockfd);
            sockfd = -1;
        }
        
    } // Closes socket

    bool SafeSocket::isConnected() const{
        return sockfd >= 0;
    }
    int SafeSocket::getFd() const{
        return sockfd;
    }

}