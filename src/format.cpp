#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {           
    constexpr long secInMin = 60;
    constexpr long secInHr = 3600;

    const long hours = seconds/secInHr;
    seconds = seconds % secInHr;
    const long minutes = seconds/secInMin;
    seconds = seconds % secInMin;

    return std::to_string(hours).append(":")
    .append(std::to_string(minutes)).append(":")
    .append(std::to_string(seconds));
}