#pragma once

#include "ILogSink.hpp"
#include <fstream>
#include <string>

namespace logging
{
    class FileSinkImpl : public ILogSink
    {
    private:
        /* data */
        std::string file_path;
        std::ofstream file;     

    public:
        FileSinkImpl(const std::string &filepath);
        void write(const LogMessage &msg) override;
        ~FileSinkImpl() override = default;
    };

} // namespace logging
