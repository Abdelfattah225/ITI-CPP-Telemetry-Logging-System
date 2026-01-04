#include "FileSinkImpl.hpp"
#include <iostream>

namespace logging
{

    FileSinkImpl::FileSinkImpl(const std::string &filepath) : file_path{filepath}
    {
        file.open(file_path, std::ios::out | std::ios::app);
        if (!file.is_open())
            std::cerr << "Failed to open: " << file_path << std::endl;
    }

    void FileSinkImpl::write(const LogMessage &msg)
    {
        if (file.is_open())
            file << msg << std::endl;
        else
            std::cerr << "Can not write, Failed to open: " << file_path << std::endl;
    }
}