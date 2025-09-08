// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/mesh.hpp>
#include <memory>

namespace QCE {
    std::unique_ptr<Mesh> generate_cuboid(float length, float width, float height) {
        std::stringstream ss_name;
        ss_name << "Cuboid[" << std::fixed << std::setprecision(4)
                << length << ";" << width << ";" << height << "]";

        auto result = std::make_unique<Mesh>(
            /*name*/ ss_name.str(),
            /*vertices*/ std::vector<vertex>{
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
            },
            /*indices*/ std::vector<uint32_t>{
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
            }
        );

        return result;
    }

}
