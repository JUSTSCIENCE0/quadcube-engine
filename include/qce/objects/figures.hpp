// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/mesh.hpp>

#include <memory>
#include <variant>

namespace QCE {
    struct CuboidParams {
        float length = 1.0;
        float width = 1.0;
        float height = 1.0;
    };
    struct SphereParams {
        float radius = 1.0;
        int   subdivisions = 1;
        bool  hard_edges = false;
    };
    using FigureParams = std::variant<CuboidParams, SphereParams>;

    Mesh generate_figure(const FigureParams& params, const std::string& name = "");
}
