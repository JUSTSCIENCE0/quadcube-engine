// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/mesh.hpp>
#include <memory>

namespace QCE {
    std::unique_ptr<Mesh> generate_cuboid(float length, float width, float height) {
        auto result = std::make_unique_for_overwrite<Mesh>();

        result->vertices = {
            {
                .position = { -length / 2, -width / 2, -height / 2 }
            },
            {
                .position = {  length / 2, -width / 2, -height / 2 }
            },
            {
                .position = {  length / 2, -width / 2,  height / 2 }
            },
            {
                .position = { -length / 2, -width / 2,  height / 2 }
            },
            {
                .position = { -length / 2,  width / 2, -height / 2 }
            },
            {
                .position = {  length / 2,  width / 2, -height / 2 }
            },
            {
                .position = {  length / 2,  width / 2,  height / 2 }
            },
            {
                .position = { -length / 2,  width / 2,  height / 2 }
            },
        };
        result->indices = {
            0, 1, 2,
            0, 2, 3,
            1, 5, 6,
            1, 6, 2,
            3, 2, 6,
            3, 6, 7,
            4, 0, 3,
            4, 3, 7,
            4, 5, 1,
            4, 1, 0,
            5, 4, 7,
            5, 7, 6
        };

        return result;
    }

}
