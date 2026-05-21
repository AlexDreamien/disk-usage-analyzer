#include "doctest.h"

#include "dux/report.hpp"

using dux::Entry;
using dux::top_n;

static std::vector<Entry> sample() {
    return {
        {"/a", 100},
        {"/b", 1000},
        {"/c", 50},
        {"/d", 750},
        {"/e", 250},
    };
}

TEST_CASE("top_n returns the largest entries sorted descending") {
    const auto result = top_n(sample(), 3);
    REQUIRE(result.size() == 3);
    CHECK(result[0].size == 1000);
    CHECK(result[1].size == 750);
    CHECK(result[2].size == 250);
}

TEST_CASE("top_n clamps to the available count") {
    const auto result = top_n(sample(), 99);
    REQUIRE(result.size() == 5);
    CHECK(result.front().size == 1000);
    CHECK(result.back().size == 50);
}

TEST_CASE("top_n with n=0 returns nothing") {
    CHECK(top_n(sample(), 0).empty());
}

TEST_CASE("top_n on empty input returns empty") {
    CHECK(top_n({}, 3).empty());
}

TEST_CASE("top_n preserves ties without crashing") {
    std::vector<Entry> entries = {{"/a", 50}, {"/b", 50}, {"/c", 50}};
    const auto result = top_n(entries, 2);
    CHECK(result.size() == 2);
    CHECK(result[0].size == 50);
    CHECK(result[1].size == 50);
}
