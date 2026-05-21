#pragma once

#include <cstdint>
#include <string>

namespace dux {

using FileSize = std::uintmax_t;

// Format a byte count as a human-readable string using binary units.
//
// Bytes are printed as a plain integer (e.g. "512 B"); anything 1 KiB or
// larger is shown with two decimal places (e.g. "1.23 MB"). Units go up
// through KB / MB / GB / TB / PB. The "KB / MB / ..." labels denote
// 1024-based ("KiB / MiB / ...") amounts — this is conventional for
// du-style tools.
std::string human_size(FileSize bytes);

}  // namespace dux
