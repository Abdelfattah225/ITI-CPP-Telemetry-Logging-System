#pragma once

#include "LogMessage.hpp"

namespace logging
{

    class ILogSink
    {
    public:
        ILogSink() = default;
        virtual void write(const LogMessage &msg) = 0;
        virtual ~ILogSink() = default;
    };

}
