#pragma once

#include <optional>
#include <charconv>
#include <string>
#include "Policies.hpp"
#include "Enums.hpp"

namespace logging {

std::optional<float> parseFloat(const std::string &str)
{
    if (str.empty()) return std::nullopt;
    
    float value;
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    
    if (ec == std::errc())
    {
        return value;
    }
    return std::nullopt;
}

template<typename Policy>
std::optional<SeverityLvl_enum> parseAndClassify(const std::string& str)
{
    auto val = parseFloat(str);
    if (val)
        return Policy::inferSeverity(*val);

    return std::nullopt;
}

} // namespace logging