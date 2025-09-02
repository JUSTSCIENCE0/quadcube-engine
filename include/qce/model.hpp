// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/mesh.hpp>

#include <memory>

namespace QCE {
    struct Model {
        std::shared_ptr<Mesh> mesh;
    };
}
