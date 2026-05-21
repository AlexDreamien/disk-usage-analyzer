# disk-usage-analyzer

[![CI](https://github.com/AlexDreamien/disk-usage-analyzer/actions/workflows/ci.yml/badge.svg)](https://github.com/AlexDreamien/disk-usage-analyzer/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/)

CLI tool in the spirit of `du` / `ncdu`: scans a directory and shows what is
taking up space — the largest files and subdirectories, sorted, in
human-readable sizes. Built with C++17 `std::filesystem` and CMake.

> _Terminal screenshot lands in this file once captured._
>
> ![Screenshot placeholder](docs/screenshot.png)

## Features

- Recursive scan via `std::filesystem`, cross-platform (Linux + Windows).
- Top-N largest **files** and top-N largest **subdirectories**, sorted
  descending.
- Human-readable sizes (`1.23 MB`, `512 KB`, `456 B`) using binary units.
- `--depth N` filters which directory totals are reported without
  truncating the scan, so totals stay accurate even at depth 1.
- `--files-only` skips the directory section entirely.
- Permission-denied and unreadable entries are skipped and counted, not
  fatal.

## Building

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

Requires CMake 3.20+ and a C++17 compiler (MSVC 2019+, GCC 8+, Clang 7+).
doctest is vendored, no external dependencies.

## Usage

```text
Usage: dux <path> [options]

Options:
  --top N           Show the top N entries (default: 20).
  --files-only      Hide the directory section; show only files.
  --depth N         Only report directories at most N levels deep.
                    Files are always reported regardless of depth.
  -h, --help        Show this help and exit.
```

### Example

```bash
$ dux ./build --top 5
Total: 5.01 MB  (173 files, 99 directories reported)

Top files:
  1.61 MB    ./build/tests/dux_tests.dir/Release/test_main.obj
  600.37 KB  ./build/tests/dux_tests.dir/Release/test_scan.obj
  504.38 KB  ./build/Release/dux_core.lib
  314.11 KB  ./third_party/doctest/doctest.h
  273.50 KB  ./build/tests/Release/dux_tests.exe

Top directories:
  4.58 MB    ./build
  2.98 MB    ./build/tests
  2.67 MB    ./build/tests/dux_tests.dir/Release
  2.67 MB    ./build/tests/dux_tests.dir
  562.38 KB  ./build/Release
```

## Architecture

```
include/dux/
  format.hpp     human_size() — bytes -> "1.23 MB"
  scan.hpp       scan_directory() returns ScanResult{files, dirs,
                 total_bytes, errors}
  report.hpp     top_n() — sort and pick the largest entries
src/             Implementations of the above
src/main.cpp     CLI: argv parsing, output formatting
tests/           doctest suite over a temp-dir fixture
third_party/     Vendored doctest single-header
```

The interesting design choice is in `scan.cpp`: the recursive walk always
descends fully so directory totals are correct, even when `--depth N`
limits how many of those directories the report mentions. Each
directory's recursive size rolls up to its parent, so a deep file is
reflected in every ancestor's total.

## Tests

17 doctest cases / 59 assertions covering:

- `format` — bytes / KB / MB / GB / TB / PB, fractional formatting,
  PB-scale values without overflow
- `scan` — empty dir, flat sums, multi-level rollups, ancestor
  propagation, `max_report_depth` filter semantics, nonexistent root
- `report` — `top_n` correctness, count clamping, `n=0`, empty input, ties

CI runs `cmake --build` and `ctest --output-on-failure` on both Ubuntu and
Windows — see [`.github/workflows/ci.yml`](.github/workflows/ci.yml).

## Out of scope

By design: no interactive TUI (use `ncdu` for that), no file deletion (we
only report), no graphical output.

## License

[MIT](LICENSE). Vendored doctest keeps its upstream MIT license — see
[`third_party/README.md`](third_party/README.md).
