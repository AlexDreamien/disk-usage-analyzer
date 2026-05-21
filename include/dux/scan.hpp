#pragma once

#include <cstddef>
#include <filesystem>
#include <vector>

#include "dux/format.hpp"

namespace dux {

struct Entry {
    std::filesystem::path path;
    FileSize size{0};
};

struct ScanResult {
    std::vector<Entry> files;       // every regular file under root
    std::vector<Entry> dirs;        // subdirectories with their recursive size
    FileSize total_bytes{0};        // recursive size of root itself
    std::size_t errors{0};          // entries we couldn't stat (permission etc.)
};

// Recursively scan `root`. Directories are always traversed to compute
// totals, but only those at depth <= max_report_depth appear in
// `dirs` (use -1 for "no limit"). The `files` list is always exhaustive.
ScanResult scan_directory(const std::filesystem::path& root, int max_report_depth = -1);

}  // namespace dux
