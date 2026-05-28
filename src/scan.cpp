#include "dux/scan.hpp"

#include <system_error>

namespace dux {

namespace fs = std::filesystem;

namespace {

// Returns the recursive size of the subtree rooted at `dir`.
FileSize scan_subtree(const fs::path& dir,
                      int depth_from_root,
                      int max_report_depth,
                      ScanResult& out) {
    std::error_code ec;
    fs::directory_iterator it(dir, fs::directory_options::skip_permission_denied, ec);
    if (ec) {
        ++out.errors;
        return 0;
    }

    FileSize subtree_total = 0;
    const fs::directory_iterator end;
    while (it != end) {
        auto sym_status = it->symlink_status(ec);
        if (ec) {
            ++out.errors;
            ec.clear();
            it.increment(ec);
            ec.clear();
            continue;
        }

        if (fs::is_symlink(sym_status)) {
            // Never follow symlinks (matches du default behaviour); this also
            // prevents infinite recursion when a symlink points to an ancestor.
        } else if (fs::is_regular_file(sym_status)) {
            const auto sz = it->file_size(ec);
            if (ec) {
                ++out.errors;
                ec.clear();
            } else {
                subtree_total += sz;
                out.files.push_back({it->path(), sz});
            }
        } else if (fs::is_directory(sym_status)) {
            const FileSize child_total = scan_subtree(
                it->path(), depth_from_root + 1, max_report_depth, out);
            subtree_total += child_total;
            if (max_report_depth < 0 || depth_from_root + 1 <= max_report_depth) {
                out.dirs.push_back({it->path(), child_total});
            }
        }

        it.increment(ec);
        if (ec) {
            ++out.errors;
            ec.clear();
        }
    }
    return subtree_total;
}

}  // namespace

ScanResult scan_directory(const fs::path& root, int max_report_depth) {
    ScanResult result;
    std::error_code ec;
    if (!fs::exists(root, ec) || !fs::is_directory(root, ec)) {
        ++result.errors;
        return result;
    }
    result.total_bytes = scan_subtree(root, 0, max_report_depth, result);
    return result;
}

}  // namespace dux
