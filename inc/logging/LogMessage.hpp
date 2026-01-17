#pragma once

#include <chrono>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <ostream>

namespace logging
{

    enum class Severity
    {
        INFO,
        WARN,
        CRITICAL
    };

    enum class Context
    {
        CPU,
        GPU,
        RAM
    };

    using TimeStamp = std::chrono::system_clock::time_point;

    class LogMessage
    {
    private:
        std::string app_name;
        TimeStamp time;
        Context context;
        Severity severity;
        uint8_t payload;
        std::string text;

        void AssignSeverity()
        {
            if (payload <= 25)
            {
                severity = Severity::INFO;
            }
            else if (payload > 25 && payload < 75)
            {
                severity = Severity::WARN;
            }
            else if (payload >= 75)
            {
                severity = Severity::CRITICAL;
            }
        }

        std::string contextToString(Context ctx) const
        {
            switch (ctx)
            {
            case Context::CPU:
                return "CPU";
            case Context::GPU:
                return "GPU";
            case Context::RAM:
                return "RAM";
            default:
                return "UNKNOWN";
            }
        }
        std::string severityToString(Severity sev) const
        {
            switch (sev)
            {
            case Severity::INFO:
                return "INFO";
            case Severity::WARN:
                return "WARN";
            case Severity::CRITICAL:
                return "CRITICAL";
            default:
                return "UNKNOWN";
            }
        }

        std::string timeToString(const TimeStamp &time) const
        {
            // Step 1: Convert chrono to time_t
            std::time_t tt = std::chrono::system_clock::to_time_t(time);

            // Step 2: Convert time_t to tm (local time)
            std::tm *local = std::localtime(&tt);

            // Step 3: Format to string
            std::stringstream ss;
            ss << std::put_time(local, "%Y-%m-%d %H:%M:%S");

            return ss.str();
        }

    public:
        LogMessage(std::string application_name, Context cxt, uint8_t Payload) : app_name{application_name}, context{cxt}, payload{Payload}
        {
            // determine which serverity based on payload
            AssignSeverity();
            time = std::chrono::system_clock::now();
            std::string TimeStampformated = timeToString(time);
            std::string severityformated = severityToString(severity);
            std::string contextformated = contextToString(context);

            text = "[" + TimeStampformated + "]" + " [" + contextformated + "]" + " [" + app_name + "]" + " [" + severityformated + "]" + " Payload value is: " + std::to_string(payload) + "%";
        }
         // DEFAULT ALL SPECIAL MEMBER FUNCTIONS (Rule of 0 approach)   
        ~LogMessage() = default;                             // Destructor
        LogMessage(const LogMessage &) = default;            // Copy constructor
        LogMessage &operator=(const LogMessage &) = default; // Copy assignment
        LogMessage(LogMessage &&) = default;                 // Move constructor
        LogMessage &operator=(LogMessage &&) = default;      // Move assignment
        const std::string &getAppName() const
        {
            return app_name;
        }
        const TimeStamp &getTime() const
        {
            return time;
        }
        const Context &getContext() const
        {
            return context;
        }
        const Severity &getSeverity() const
        {
            return severity;
        }
        const std::string &getText() const
        {
            return text;
        }

        friend std::ostream &operator<<(std::ostream &os, const LogMessage &msg);
    };

    // Can access private because we're friends!
    inline std::ostream &operator<<(std::ostream &os, const LogMessage &msg)
    {
        os << msg.text;
        return os;
    }

}