// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/error_codes.hpp>
#include <qce/objects/mesh.hpp>

#include <filesystem>
#include <vector>

namespace QCE {
    ErrorCode load_obj_file(const std::filesystem::path& path, std::vector<Mesh>& meshes);
}
