#pragma once

#include "FileTelemetrySourceImpl.hpp"
#include <string>

namespace SmartDataHub
{

class TelemetryParser
{
private:
    // Data sources (using YOUR existing class!)
    FileTelemetrySourceImpl m_cpuSource;
    FileTelemetrySourceImpl m_memSource;

    // CPU stats structure (for tracking between reads)
    struct CpuStats
    {
        unsigned long long user = 0;
        unsigned long long nice = 0;
        unsigned long long system = 0;
        unsigned long long idle = 0;
        unsigned long long iowait = 0;
        unsigned long long irq = 0;
        unsigned long long softirq = 0;
        unsigned long long steal = 0;

        unsigned long long getTotal() const;
        unsigned long long getIdle() const;
    };

    CpuStats m_prevCpu;
    CpuStats m_currCpu;
    bool m_firstRead = true;

    // Helper methods
    bool parseCpuLine(const std::string& line);
    bool parseMemLine(const std::string& line, unsigned long long& memTotal, 
                      unsigned long long& memAvailable);

public:
    TelemetryParser();

    // Rule of 0: No special member functions!

    // Open sources
    bool open();

    // Get usage percentages
    double getCpuUsage();
    double getMemUsage();

    // Get formatted strings
    std::string getCpuString();
    std::string getMemString();
};

}  // namespace SmartDataHub