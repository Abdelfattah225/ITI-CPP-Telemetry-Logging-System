#pragma once

#include "ITelemetrySource.hpp"
#include "SafeSocket.hpp"

namespace SmartDataHub
{

    class SocketTelemetrySourceImpl : public ITelemetrySource
    {
    private:
        SafeSocket m_socket;
        std::string m_socketpath;

    public:
        explicit SocketTelemetrySourceImpl(const std::string &path);
        // Rule of 0: NO special member functions
        // Compiler generates them using SafeFile's Rule of 5

        // ITelemetrySource interface implementation
        bool openSource() override;
        bool readSource(std::string &out) override;
    };

} // SmartDataHub
