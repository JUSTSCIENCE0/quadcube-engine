// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/mesh.hpp>

#include <memory>
#include <iomanip>
#include <variant>

namespace QCE {
    struct CuboidParams {
        float length = 1.0;
        float width = 1.0;
        float height = 1.0;
        int   subdivisions = 1;
    };
    struct SphereParams {
        float radius = 1.0;
        int   subdivisions = 1;
    };
    using FigureParams = std::variant<CuboidParams, SphereParams>;

    std::unique_ptr<Mesh> generate_cuboid(
            float length, float width, float height,
            /*TODO: int subdivisions = 1 ,*/
            std::string name = "") {
        if (name.empty()) {
            std::stringstream ss_name;
            ss_name << "Cuboid[" << std::fixed << std::setprecision(4)
                << length << ";" << width << ";" << height << "]";
            name = ss_name.str();
        }

        float l2 = length / 2.0f;
        float w2 = width  / 2.0f;
        float h2 = height / 2.0f;

        auto result = std::make_unique<Mesh>(
            /*name*/ std::move(name),
            /*vertices*/ std::vector<vertex>{
                {
                    .position = { -l2, -w2, -h2 }
                },
                {
                    .position = {  l2, -w2, -h2 }
                },
                {
                    .position = {  l2, -w2,  h2 }
                },
                {
                    .position = { -l2, -w2,  h2 }
                },
                {
                    .position = { -l2,  w2, -h2 }
                },
                {
                    .position = {  l2,  w2, -h2 }
                },
                {
                    .position = {  l2,  w2,  h2 }
                },
                {
                    .position = { -l2,  w2,  h2 }
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

    std::unique_ptr<Mesh> create_figure(const FigureParams& params, const std::string& name = "") {
        if (std::holds_alternative<CuboidParams>(params)) {
            const auto& cube_params = std::get<CuboidParams>(params);
            return generate_cuboid(
                cube_params.length,
                cube_params.width,
                cube_params.height,
                name);
        }

        if (std::holds_alternative<SphereParams>(params)) {
            // TODO
            //const auto& sphere_params = std::get<SphereParams>(params);
            return nullptr;
        }

        return nullptr;
    }
}
