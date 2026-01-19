
#include "TelemetryParser.hpp"
#include <sstream>
#include <iomanip>

namespace SmartDataHub
{

// ══════════════════════════════════════════════════════════════════════
// CpuStats Helper Methods
// ══════════════════════════════════════════════════════════════════════

unsigned long long TelemetryParser::CpuStats::getTotal() const
{
    return user + nice + system + idle + iowait + irq + softirq + steal;
}

unsigned long long TelemetryParser::CpuStats::getIdle() const
{
    return idle + iowait;
}

// ══════════════════════════════════════════════════════════════════════
// Constructor
// ══════════════════════════════════════════════════════════════════════

TelemetryParser::TelemetryParser()
    : m_cpuSource("/proc/stat"),
      m_memSource("/proc/meminfo")
{
    // Sources initialized with paths
}

// ══════════════════════════════════════════════════════════════════════
// Private Helper Methods
// ══════════════════════════════════════════════════════════════════════

bool TelemetryParser::parseCpuLine(const std::string& line)
{
    std::istringstream iss(line);
    std::string cpuLabel;

    // Parse: "cpu  user nice system idle iowait irq softirq steal"
    iss >> cpuLabel
        >> m_currCpu.user
        >> m_currCpu.nice
        >> m_currCpu.system
        >> m_currCpu.idle
        >> m_currCpu.iowait
        >> m_currCpu.irq
        >> m_currCpu.softirq
        >> m_currCpu.steal;

    return !iss.fail();
}

// ══════════════════════════════════════════════════════════════════════
// CPU Usage
// ══════════════════════════════════════════════════════════════════════

double TelemetryParser::getCpuUsage()
{
    // Open source fresh (values change constantly)
    if (!m_cpuSource.openSource())
    {
        return -1.0;  // Error
    }

    // Read first line
    std::string line;
    if (!m_cpuSource.readSource(line))
    {
        return -1.0;  // Error
    }

    // Save previous stats
    m_prevCpu = m_currCpu;

    // Parse new stats
    if (!parseCpuLine(line))
    {
        return -1.0;  // Parse error
    }

    // First read - no previous data to compare
    if (m_firstRead)
    {
        m_firstRead = false;
        return 0.0;
    }

    // Calculate usage
    unsigned long long totalDiff = m_currCpu.getTotal() - m_prevCpu.getTotal();
    unsigned long long idleDiff = m_currCpu.getIdle() - m_prevCpu.getIdle();

    if (totalDiff == 0)
    {
        return 0.0;
    }

    return ((double)(totalDiff - idleDiff) / totalDiff) * 100.0;
}

// ══════════════════════════════════════════════════════════════════════
// Memory Usage
// ══════════════════════════════════════════════════════════════════════

double TelemetryParser::getMemUsage()
{
    // Open source fresh
    if (!m_memSource.openSource())
    {
        return -1.0;  // Error
    }

    unsigned long long memTotal = 0;
    unsigned long long memAvailable = 0;
    bool foundTotal = false;
    bool foundAvailable = false;

    // Read lines until we find both values
    std::string line;
    while (m_memSource.readSource(line))
    {
        // Parse MemTotal
        if (line.find("MemTotal:") == 0)
        {
            std::istringstream iss(line);
            std::string label;
            iss >> label >> memTotal;
            foundTotal = true;
        }
        // Parse MemAvailable
        else if (line.find("MemAvailable:") == 0)
        {
            std::istringstream iss(line);
            std::string label;
            iss >> label >> memAvailable;
            foundAvailable = true;
        }

        // Stop if we found both
        if (foundTotal && foundAvailable)
        {
            break;
        }
    }

    // Check if we found both values
    if (!foundTotal || !foundAvailable || memTotal == 0)
    {
        return -1.0;  // Error
    }

    // Calculate usage percentage
    double used = static_cast<double>(memTotal - memAvailable);
    return (used / static_cast<double>(memTotal)) * 100.0;
}

// ══════════════════════════════════════════════════════════════════════
// Formatted Output Strings
// ══════════════════════════════════════════════════════════════════════

std::string TelemetryParser::getCpuString()
{
    double usage = getCpuUsage();
    
    if (usage < 0)
    {
        return "CPU: Error reading data";
    }

    std::ostringstream oss;
    oss << "CPU: " << std::fixed << std::setprecision(1) << usage << "%";
    return oss.str();
}

std::string TelemetryParser::getMemString()
{
    double usage = getMemUsage();
    
    if (usage < 0)
    {
        return "Memory: Error reading data";
    }

    std::ostringstream oss;
    oss << "Memory: " << std::fixed << std::setprecision(1) << usage << "%";
    return oss.str();
}

}  // namespace SmartDataHub