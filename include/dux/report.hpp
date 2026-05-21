#pragma once

#include <cstddef>
#include <vector>

#include "dux/scan.hpp"

namespace dux {

// Sort `entries` in descending size order and return at most `n` of the
// largest. The input vector is taken by value because it is rearranged.
std::vector<Entry> top_n(std::vector<Entry> entries, std::size_t n);

}  // namespace dux
