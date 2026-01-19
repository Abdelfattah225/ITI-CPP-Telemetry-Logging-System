#include "FileTelemetrySourceImpl.hpp"

namespace SmartDataHub
{

    FileTelemetrySourceImpl::FileTelemetrySourceImpl(const std::string &path) : m_filepath{path}
    {
    }
    // Rule of 0: NO special member functions
    // Compiler generates them using SafeFile's Rule of 5

    // ITelemetrySource interface implementation
    bool FileTelemetrySourceImpl::openSource()
    {
       return m_file.openFile(m_filepath.c_str(), O_RDONLY);
    }
    bool FileTelemetrySourceImpl::readSource(std::string &out)
    {
        return m_file.isOpen() ?  m_file.readline(out) : false  ;
    }

}