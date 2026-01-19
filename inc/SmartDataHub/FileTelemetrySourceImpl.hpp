#pragma once

#include "ITelemetrySource.hpp"
#include "SafeFile.hpp"
#include <fcntl.h>
namespace SmartDataHub
{

    class FileTelemetrySourceImpl : public ITelemetrySource
    {
    private:
        SafeFile m_file;        // RAII wrapper
        std::string m_filepath; // path to telemetry file
    public:
        explicit FileTelemetrySourceImpl(const std::string &path);
        // Rule of 0: NO special member functions
        // Compiler generates them using SafeFile's Rule of 5

        // ITelemetrySource interface implementation
        bool openSource() override;
        bool readSource(std::string &out) override;
    };

} // SmartDataHub