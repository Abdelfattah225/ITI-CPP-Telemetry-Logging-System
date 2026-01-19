#pragma once


#include <string>
namespace SmartDataHub {

class ITelemetrySource
{
public:

    virtual bool openSource() = 0;
    virtual bool readSource(std::string& out) = 0;
    virtual ~ITelemetrySource() = default;
};
} // namespace SmartDataHub 