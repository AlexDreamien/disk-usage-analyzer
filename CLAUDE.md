# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

`du`/`ncdu`-style CLI: scans a directory and reports the largest files and subdirectories. C++17 + CMake. See `README.md` for usage.

## Build & test

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build --output-on-failure
ctest --test-dir build -R <regex>          # single test / subset
```

CMake 3.20+, C++17 compiler. doctest is vendored in `third_party/` — no external deps.

## Architecture invariant

`include/dux/` splits the three concerns: `format` (`human_size`), `scan` (`scan_directory` → `ScanResult`), `report` (`top_n`). Keep them independent; `src/main.cpp` is CLI glue only.

## Gotchas — do not regress

- **Symlinks are never followed** (matches `du` default). `scan_subtree` uses `it->symlink_status()` and skips symlinks — do **not** switch back to `it->status()`, which dereferences symlinks and causes **infinite recursion / stack overflow** on a symlink pointing to an ancestor.
- **The walk always descends fully; `--depth N` only limits what is *reported*, not what is *scanned*.** This keeps directory totals accurate even at depth 1. Don't prune the recursion to implement `--depth`.
- **Directory totals roll up to every ancestor** — a deep file is reflected in each parent's size.
- **`--top` rejects negative input** before `std::stoull` (which would otherwise wrap `-1` to a huge number).
- Permission-denied / unreadable entries are counted in `errors` and skipped, never fatal.
