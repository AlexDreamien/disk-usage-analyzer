#include "dux/report.hpp"

#include <algorithm>

namespace dux {

std::vector<Entry> top_n(std::vector<Entry> entries, std::size_t n) {
    if (n == 0) return {};
    if (n >= entries.size()) {
        std::sort(entries.begin(), entries.end(),
                  [](const Entry& a, const Entry& b) { return a.size > b.size; });
        return entries;
    }
    std::partial_sort(
        entries.begin(), entries.begin() + static_cast<std::ptrdiff_t>(n), entries.end(),
        [](const Entry& a, const Entry& b) { return a.size > b.size; });
    entries.resize(n);
    return entries;
}

}  // namespace dux
