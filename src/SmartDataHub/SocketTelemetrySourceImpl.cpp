#include "SocketTelemetrySourceImpl.hpp"

namespace SmartDataHub
{
    SocketTelemetrySourceImpl::SocketTelemetrySourceImpl(const std::string &path) : m_socketpath{path}
    {
    }
    bool SocketTelemetrySourceImpl::openSource()
    {
        if (!m_socket.createSocket())
        {
            return false;
        }
        return m_socket.connectSocket(m_socketpath);
    }
    bool SocketTelemetrySourceImpl::readSource(std::string &out)
    {
        if(!m_socket.isConnected()){
            return false;
        }
        int bytes = m_socket.receiveData(out);
        return bytes > 0 ;
    }
}