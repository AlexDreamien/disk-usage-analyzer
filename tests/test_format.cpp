#include "doctest.h"

#include "dux/format.hpp"

using dux::human_size;

TEST_CASE("bytes below 1 KB stay raw") {
    CHECK(human_size(0) == "0 B");
    CHECK(human_size(1) == "1 B");
    CHECK(human_size(512) == "512 B");
    CHECK(human_size(1023) == "1023 B");
}

TEST_CASE("kilobytes round up at 1024") {
    CHECK(human_size(1024) == "1.00 KB");
    CHECK(human_size(1024 + 512) == "1.50 KB");
    CHECK(human_size(2 * 1024) == "2.00 KB");
}

TEST_CASE("scales up through MB, GB, TB") {
    CHECK(human_size(1024ULL * 1024) == "1.00 MB");
    CHECK(human_size(1024ULL * 1024 * 1024) == "1.00 GB");
    CHECK(human_size(1024ULL * 1024 * 1024 * 1024) == "1.00 TB");
}

TEST_CASE("formats fractional values with two decimals") {
    CHECK(human_size(1024ULL * 1024 * 3 / 2) == "1.50 MB");
    CHECK(human_size(static_cast<dux::FileSize>(1024 * 1024 * 1.23456)) == "1.23 MB");
}

TEST_CASE("very large values stay in PB without overflowing") {
    const dux::FileSize pb = 1024ULL * 1024 * 1024 * 1024 * 1024;
    CHECK(human_size(pb) == "1.00 PB");
    CHECK(human_size(2 * pb) == "2.00 PB");
}
