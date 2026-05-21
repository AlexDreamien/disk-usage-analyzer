#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "dux/format.hpp"
#include "dux/report.hpp"
#include "dux/scan.hpp"

namespace fs = std::filesystem;
using namespace dux;

namespace {

constexpr const char* USAGE =
    "Usage: dux <path> [options]\n"
    "\n"
    "Scan a directory and report the largest files and subdirectories.\n"
    "\n"
    "Options:\n"
    "  --top N           Show the top N entries (default: 20).\n"
    "  --files-only      Hide the directory section; show only files.\n"
    "  --depth N         Only report directories at most N levels deep.\n"
    "                    Files are always reported regardless of depth.\n"
    "  -h, --help        Show this help and exit.\n";

struct Args {
    fs::path root;
    std::size_t top = 20;
    bool files_only = false;
    int depth = -1;
};

std::string_view require_value(int argc, char** argv, int& i) {
    if (i + 1 >= argc) {
        throw std::invalid_argument(std::string("Missing value for ") + argv[i]);
    }
    return argv[++i];
}

Args parse_args(int argc, char** argv) {
    Args args;
    bool root_set = false;
    for (int i = 1; i < argc; ++i) {
        const std::string_view a = argv[i];
        if (a == "-h" || a == "--help") {
            std::cout << USAGE;
            std::exit(0);
        } else if (a == "--top") {
            args.top = static_cast<std::size_t>(
                std::stoull(std::string(require_value(argc, argv, i))));
        } else if (a == "--files-only") {
            args.files_only = true;
        } else if (a == "--depth") {
            args.depth = std::stoi(std::string(require_value(argc, argv, i)));
            if (args.depth < 0) {
                throw std::invalid_argument("--depth must be >= 0");
            }
        } else if (!a.empty() && a[0] == '-') {
            throw std::invalid_argument("Unknown option: " + std::string(a));
        } else if (!root_set) {
            args.root = std::string(a);
            root_set = true;
        } else {
            throw std::invalid_argument("Unexpected positional argument: " + std::string(a));
        }
    }
    if (!root_set) {
        throw std::invalid_argument("Path is required. Run with --help for usage.");
    }
    return args;
}

void print_section(std::ostream& out,
                   const std::string& heading,
                   const std::vector<Entry>& entries) {
    if (entries.empty()) return;
    out << '\n' << heading << '\n';
    for (const auto& e : entries) {
        out << "  " << human_size(e.size) << "    " << e.path.string() << '\n';
    }
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const Args args = parse_args(argc, argv);
        ScanResult result = scan_directory(args.root, args.depth);

        std::cout << "Total: " << human_size(result.total_bytes)
                  << "  (" << result.files.size() << " files, "
                  << result.dirs.size() << " directories reported)";
        if (result.errors > 0) {
            std::cout << "  [" << result.errors << " skipped]";
        }
        std::cout << '\n';

        const auto top_files = top_n(result.files, args.top);
        print_section(std::cout, "Top files:", top_files);

        if (!args.files_only) {
            const auto top_dirs = top_n(result.dirs, args.top);
            print_section(std::cout, "Top directories:", top_dirs);
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
