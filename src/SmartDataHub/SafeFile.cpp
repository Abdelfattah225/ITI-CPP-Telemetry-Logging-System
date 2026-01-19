#include "SafeFile.hpp"
#include <iostream>
#include <unistd.h>

constexpr int openFlag = O_RDWR;
SmartDataHub::SafeFile::SafeFile(const std::string &_path)
{
    SmartDataHub::SafeFile::openFile(_path.c_str(), openFlag);
}

SmartDataHub::SafeFile::SafeFile(SafeFile &&other) noexcept : _fd{other._fd} 
{
    other._fd = -1;
}
SmartDataHub::SafeFile &SmartDataHub::SafeFile::operator=(SafeFile &&other) noexcept
{

    if (this != &other)
    {
        closeFile();
        this->_fd = other._fd;
        other._fd = -1;
    }

    return *this;
}

bool SmartDataHub::SafeFile::openFile(const char *_path, const int &flags)
{
    // Close if already open
    if (_fd >= 0)
    {
        closeFile();
    }
    _fd = open(_path, flags);
    if (_fd < 0)
    {
        std::cerr << "Can open the file with fd= " << _fd << std::endl;
        return false;
    }
    else
    {
        return true;
    }
}

constexpr uint8_t NoOfChar = 1;
constexpr uint8_t InitValueForNoOFBytes = 1;

bool SmartDataHub::SafeFile::readline(std::string &line)
{

    if (SmartDataHub::SafeFile::isOpen())
    {

        std::string result = "";
        char ch;
        ssize_t NoOfBytes;
        while ((NoOfBytes = read(_fd, &ch, 1)) > 0)
        {
            if (ch == '\n')
                break; // Stop at newline
            result += ch;
        }
        if (NoOfBytes == -1)
        {
            std::cerr << "ERROR: failed to capture data from file\n";
            return false;
        }
        else
        {
            line = result;
            return true;
        }
    }
    else
    {
        std::cerr << "ERROR: The File did not open\n";
        return false;
    }
}

bool SmartDataHub::SafeFile::writeline(std::string &line)
{
    int NoOfBytes;
    if (SmartDataHub::SafeFile::isOpen())
    {
        for (char c : line)
        {
            NoOfBytes = write(_fd, &c, NoOfChar);
            if (NoOfBytes < 0)
            {
                std::cerr << "ERROR: Cannot write to the file\n";
                return false;
            }
        }
        return true;
    }
    else
    {
        std::cerr << "ERROR: The File did not open\n";
        return false;
    }
}

bool SmartDataHub::SafeFile::isOpen() const
{
   return _fd >= 0 ;
}

int SmartDataHub::SafeFile::getFd() const
{
    return this->_fd;
}
void SmartDataHub::SafeFile::closeFile()
{
    if (_fd > 0)
    {
        close(_fd);
        _fd = -1;
    }
}

void SmartDataHub::SafeFile::seekToBeginning()
{
    if (_fd >= 0)
    {
        lseek(_fd, 0, SEEK_SET);
    }
}
SmartDataHub::SafeFile::~SafeFile()
{
    SmartDataHub::SafeFile::closeFile();
}