// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/error_codes.hpp>

#include <filesystem>

namespace QCE {
    ErrorCode texture_load(const std::filesystem::path& path /*, Texture& texture*/);
}
