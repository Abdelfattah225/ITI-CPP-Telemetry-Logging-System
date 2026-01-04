#pragma once

#include "ILogSink.hpp"

namespace logging
{
    class ConsoleSinkImpl : public ILogSink
    {
    public:
        ConsoleSinkImpl() = default;
       void write(const LogMessage & msg) override ;
        ~ConsoleSinkImpl() override = default;
    };

}