#include "FileTelemetrySourceImpl.hpp"

namespace SmartDataHub
{

    FileTelemetrySourceImpl::FileTelemetrySourceImpl(const std::string &path) : m_filepath{path}
    {
    }

    bool FileTelemetrySourceImpl::openSource()
    {
        return m_file.openFile(m_filepath.c_str(), O_RDONLY);
    }

    bool FileTelemetrySourceImpl::readSource(std::string &out)
    {
        if (!m_file.isOpen())
        {
            return false;
        }

        // Reset file position to beginning before reading
        m_file.seekToBeginning();

        return m_file.readline(out);
    }

}