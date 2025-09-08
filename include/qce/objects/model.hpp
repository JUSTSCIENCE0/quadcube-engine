// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/mesh.hpp>

#include <memory>

namespace QCE {
    struct Model {
        std::string id{};

        std::shared_ptr<Mesh> mesh{};
    };
}
