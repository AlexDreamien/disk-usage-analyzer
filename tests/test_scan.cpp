#include "doctest.h"

#include <algorithm>

#include "dux/scan.hpp"
#include "test_helpers.hpp"

using namespace dux;
using namespace dux::test;

namespace {

const Entry* find(const std::vector<Entry>& v, const std::string& tail) {
    for (const auto& e : v) {
        if (e.path.filename().string() == tail) return &e;
    }
    return nullptr;
}

}  // namespace

TEST_CASE("empty directory returns zero totals and no entries") {
    TempDir tmp;
    const auto r = scan_directory(tmp.path);
    CHECK(r.files.empty());
    CHECK(r.dirs.empty());
    CHECK(r.total_bytes == 0);
    CHECK(r.errors == 0);
}

TEST_CASE("flat directory totals match the sum of file sizes") {
    TempDir tmp;
    write_file(tmp.path / "a.bin", 100);
    write_file(tmp.path / "b.bin", 250);

    const auto r = scan_directory(tmp.path);
    CHECK(r.files.size() == 2);
    CHECK(r.dirs.empty());
    CHECK(r.total_bytes == 350);
}

TEST_CASE("subdirectory totals roll up to parent") {
    TempDir tmp;
    write_file(tmp.path / "root.bin", 100);
    write_file(tmp.path / "sub" / "a.bin", 200);
    write_file(tmp.path / "sub" / "b.bin", 300);

    const auto r = scan_directory(tmp.path);
    CHECK(r.files.size() == 3);
    REQUIRE(r.dirs.size() == 1);
    CHECK(r.dirs[0].path.filename().string() == "sub");
    CHECK(r.dirs[0].size == 500);
    CHECK(r.total_bytes == 600);
}

TEST_CASE("deeper directory totals propagate up through every ancestor") {
    TempDir tmp;
    write_file(tmp.path / "a" / "b" / "c" / "deep.bin", 1000);

    const auto r = scan_directory(tmp.path);
    REQUIRE(r.files.size() == 1);
    CHECK(r.files[0].size == 1000);
    REQUIRE(r.dirs.size() == 3);

    const auto* a = find(r.dirs, "a");
    const auto* b = find(r.dirs, "b");
    const auto* c = find(r.dirs, "c");
    REQUIRE(a);
    REQUIRE(b);
    REQUIRE(c);
    CHECK(a->size == 1000);
    CHECK(b->size == 1000);
    CHECK(c->size == 1000);
    CHECK(r.total_bytes == 1000);
}

TEST_CASE("max_report_depth filters which directories appear, but not which files") {
    TempDir tmp;
    write_file(tmp.path / "a" / "b" / "c" / "deep.bin", 100);

    const auto r = scan_directory(tmp.path, /*max_report_depth=*/2);
    // All files reported regardless of depth:
    REQUIRE(r.files.size() == 1);
    // Only dirs at depth 1 and 2 appear (root-relative).
    REQUIRE(r.dirs.size() == 2);
    CHECK(find(r.dirs, "a") != nullptr);
    CHECK(find(r.dirs, "b") != nullptr);
    CHECK(find(r.dirs, "c") == nullptr);
    CHECK(r.total_bytes == 100);
}

TEST_CASE("max_report_depth=0 hides every subdirectory") {
    TempDir tmp;
    write_file(tmp.path / "sub" / "a.bin", 100);
    const auto r = scan_directory(tmp.path, /*max_report_depth=*/0);
    CHECK(r.files.size() == 1);
    CHECK(r.dirs.empty());
    CHECK(r.total_bytes == 100);
}

TEST_CASE("nonexistent root yields one error") {
    const auto r = scan_directory("/definitely/does/not/exist_dux_xyz");
    CHECK(r.files.empty());
    CHECK(r.errors >= 1);
}
