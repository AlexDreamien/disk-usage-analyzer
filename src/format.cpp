#include "dux/format.hpp"

#include <array>
#include <iomanip>
#include <sstream>

namespace dux {

std::string human_size(FileSize bytes) {
    constexpr double UNIT = 1024.0;
    static constexpr std::array<const char*, 6> UNITS = {"B", "KB", "MB", "GB", "TB", "PB"};

    double value = static_cast<double>(bytes);
    std::size_t i = 0;
    while (value >= UNIT && i + 1 < UNITS.size()) {
        value /= UNIT;
        ++i;
    }

    std::ostringstream out;
    if (i == 0) {
        out << bytes << ' ' << UNITS[0];
    } else {
        out << std::fixed << std::setprecision(2) << value << ' ' << UNITS[i];
    }
    return out.str();
}

}  // namespace dux
