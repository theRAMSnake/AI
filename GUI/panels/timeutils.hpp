#pragma once

#include <string>

template<class DurationIn, class FirstDuration, class...RestDurations>
std::string formatDuration(DurationIn d)
{   
    auto val = std::chrono::duration_cast<FirstDuration>(d);

    std::string out = std::to_string(val.count());

    if constexpr(sizeof...(RestDurations) > 0) {
        out += "::" + formatDuration<DurationIn, RestDurations...>(d - val);
    }

    return out;
}

template<class DurationIn>
std::string formatDuration(DurationIn) { return {}; } // recursion termination