// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/objects/figures.hpp>

#include <iomanip>
#include <sstream>

namespace QCE {
    Mesh generate_cuboid(
        float length, float width, float height,
        /*TODO: int subdivisions,*/
        std::string name) {
        if (name.empty()) {
            std::stringstream ss_name;
            ss_name << "Cuboid[" << std::fixed << std::setprecision(4)
                << length << ";" << width << ";" << height << "]";
            name = ss_name.str();
        }

        float l2 = length / 2.0f;
        float w2 = width / 2.0f;
        float h2 = height / 2.0f;

        Mesh result{};
        result.id = std::move(name),
        result.vertices = std::vector<vertex>{
            // front face
            {
                .position = { -w2, -h2, -l2 },
                .normal = { 0.0f, 0.0f, -1.0f },
                .texture_coordinates = { 0.0f, 0.5f }
            },
            {
                .position = { -w2,  h2, -l2 },
                .normal = { 0.0f, 0.0f, -1.0f },
                .texture_coordinates = { 0.0f, 0.0f }
            },
            {
                .position = {  w2,  h2, -l2 },
                .normal = { 0.0f, 0.0f, -1.0f },
                .texture_coordinates = { 0.5f, 0.0f }
            },
            {
                .position = {  w2, -h2, -l2 },
                .normal = { 0.0f, 0.0f, -1.0f },
                .texture_coordinates = { 0.5f, 0.5f }
            },

            // back face
            {
                .position = { -w2, -h2,  l2 },
                .normal = { 0.0f, 0.0f, 1.0f },
                .texture_coordinates = { 0.5f, 0.5f }
            },
            {
                .position = {  w2, -h2,  l2 },
                .normal = { 0.0f, 0.0f, 1.0f },
                .texture_coordinates = { 0.0f, 0.5f }
            },
            {
                .position = {  w2,  h2,  l2 },
                .normal = { 0.0f, 0.0f, 1.0f },
                .texture_coordinates = { 0.0f, 0.0f }
            },
            {
                .position = { -w2,  h2,  l2 },
                .normal = { 0.0f, 0.0f, 1.0f },
                .texture_coordinates = { 0.5f, 0.0f }
            },

            // top face
             {
                .position = { -w2,  h2, -l2 },
                .normal = { 0.0f, 1.0f, 0.0f },
                .texture_coordinates = { 0.5f, 0.5f }
            },
            {
                .position = { -w2,  h2,  l2 },
                .normal = { 0.0f, 1.0f, 0.0f },
                .texture_coordinates = { 0.5f, 0.0f }
            },
            {
                .position = {  w2,  h2,  l2 },
                .normal = { 0.0f, 1.0f, 0.0f },
                .texture_coordinates = { 1.0f, 0.0f }
            },
            {
                .position = {  w2,  h2, -l2 },
                .normal = { 0.0f, 1.0f, 0.0f },
                .texture_coordinates = { 1.0f, 0.5f }
            },

            // bottom face
            {
                .position = { -w2, -h2, -l2 },
                .normal = { 0.0f, -1.0f, 0.0f },
                .texture_coordinates = { 1.0f, 0.5f }
            },
            {
                .position = {  w2, -h2, -l2 },
                .normal = { 0.0f, -1.0f, 0.0f },
                .texture_coordinates = { 0.5f, 0.5f }
            },
            {
                .position = {  w2, -h2,  l2 },
                .normal = { 0.0f, -1.0f, 0.0f },
                .texture_coordinates = { 0.5f, 0.0f }
            },
            {
                .position = { -w2, -h2,  l2 },
                .normal = { 0.0f, -1.0f, 0.0f },
                .texture_coordinates = { 1.0f, 0.0f }
            },

            // left face
            {
                .position = { -w2, -h2,  l2 },
                .normal = { -1.0f, 0.0f, 0.0f },
                .texture_coordinates = { 0.0f, 1.0f }
            },
            {
                .position = { -w2,  h2,  l2 },
                .normal = { -1.0f, 0.0f, 0.0f },
                .texture_coordinates = { 0.0f, 0.5f }
            },
            {
                .position = { -w2,  h2, -l2 },
                .normal = { -1.0f, 0.0f, 0.0f },
                .texture_coordinates = { 0.5f, 0.5f }
            },
            {
                .position = { -w2, -h2, -l2 },
                .normal = { -1.0f, 0.0f, 0.0f },
                .texture_coordinates = { 0.5f, 1.0f }
            },

            // right face
            {
                .position = {  w2, -h2, -l2 },
                .normal = { 1.0f, 0.0f, 0.0f },
                .texture_coordinates = { 0.5f, 1.0f }
            },
            {
                .position = {  w2,  h2, -l2 },
                .normal = { 1.0f, 0.0f, 0.0f },
                .texture_coordinates = { 0.5f, 0.5f }
            },
            {
                .position = {  w2,  h2,  l2 },
                .normal = { 1.0f, 0.0f, 0.0f },
                .texture_coordinates = { 1.0f, 0.5f }
            },
            {
                .position = {  w2, -h2,  l2 },
                .normal = { 1.0f, 0.0f, 0.0f },
                .texture_coordinates = { 1.0f, 1.0f }
            }
        };
        result.indices = std::vector<index_t>{
            // front face
             0,  1,  2,
             0,  2,  3,

            // back face
             4,  5,  6,
             4,  6,  7,

            // top face
             8,  9, 10,
             8, 10, 11,

            // bottom face
            12, 13, 14,
            12, 14, 15,

            // left face
            16, 17, 18,
            16, 18, 19,

            // right face
            20, 21, 22,
            20, 22, 23
        };

        return result;
    }

    Mesh generate_figure(const FigureParams& params, const std::string& name) {
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
            return {};
        }

        return {};
    }
}
