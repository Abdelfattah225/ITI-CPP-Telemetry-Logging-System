#pragma once

#include <unistd.h> // read, write, close, lseek
#include <fcntl.h>  // open, O_RDONLY, O_WRONLY
#include <string>
namespace SmartDataHub
{

    class SafeFile
    {
    private:
        int _fd = -1;

    public:
        // rule of 5
        SafeFile() = default;
        SafeFile(const std::string &_path);
        SafeFile(SafeFile &&other) noexcept;
        SafeFile &operator=(SafeFile &&other) noexcept;

        SafeFile(const SafeFile &other) = delete;            // No copy
        SafeFile &operator=(const SafeFile &other) = delete; // No copy

        ~SafeFile();

        // Operations
        bool openFile(const char *_path, const int &flags);
        bool readline(std::string &line);
        bool writeline(std::string &line);
        void closeFile();
        void seekToBeginning();
        // Utilities
        bool isOpen() const;
        int getFd() const;
    };

}
