#pragma once

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

namespace dux::test {

namespace fs = std::filesystem;

inline fs::path make_temp_dir(const std::string& prefix = "dux_test_") {
    static std::atomic<std::uint64_t> counter{0};
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    fs::path p = fs::temp_directory_path() /
                 (prefix + std::to_string(stamp) + "_" + std::to_string(counter.fetch_add(1)));
    fs::create_directories(p);
    return p;
}

inline void write_file(const fs::path& path, std::size_t bytes, char fill = 'x') {
    fs::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary);
    std::string buf(bytes, fill);
    out.write(buf.data(), static_cast<std::streamsize>(buf.size()));
}

struct TempDir {
    fs::path path;
    TempDir() : path(make_temp_dir()) {}
    ~TempDir() {
        std::error_code ec;
        fs::remove_all(path, ec);
    }
    TempDir(const TempDir&) = delete;
    TempDir& operator=(const TempDir&) = delete;
};

}  // namespace dux::test
