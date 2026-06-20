// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/objects/figures.hpp>

#include <iomanip>
#include <format>

namespace QCE {
    Mesh generate_cuboid(const CuboidParams& params, std::string name) {
    static inline Mesh generate_cuboid(const CuboidParams& params, std::string name) {
        if (name.empty()) {
            name = std::format("Cuboid[{:.4f}.{:.4f}.{:.4f}]", params.length, params.width, params.height);
        }

        float l2 = params.length / 2.0f;
        float w2 = params.width / 2.0f;
        float h2 = params.height / 2.0f;

        Mesh result{};
        result.id = std::move(name);
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

    static inline Mesh generate_sphere(const SphereParams& params, std::string name) {
        if (name.empty()) {
            name = std::format("Sphere[{:.4f}]", params.radius);
        }

        constexpr float K1 = 0.52573111f;
        constexpr float K2 = 0.85065081f;

        Mesh result{};
        result.id = std::move(name);
        result.vertices = std::vector<vertex>{
            {
                .position = { -K1, 0.0f, K2 },
            },
            {
                .position = { K1, 0.0f, K2 },
            },
            {
                .position = { -K1, 0.0f, -K2 },
            },
            {
                .position = { K1, 0.0f, -K2 },
            },
            {
                .position = { 0.0f, K2, K1 },
            },
            {
                .position = { 0.0f, K2, -K1 },
            },
            {
                .position = { 0.0f, -K2, K1 },
            },
            {
                .position = { 0.0f, -K2, -K1 },
            },
            {
                .position = { K2, K1, 0.0f },
            },
            {
                .position = { -K2, K1, 0.0f },
            },
            {
                .position = { K2, -K1, 0.0f },
            },
            {
                .position = { -K2, -K1, 0.0f },
            }
        };
        result.indices = std::vector<index_t>{
             1,  4, 0,    4,  9,  0,     4,  5, 9,     8, 5, 4,
             1,  8, 4,    1, 10,  8,    10,  3, 8,     8, 3, 5,
             3,  2, 5,    3,  7,  2,     3, 10, 7,    10, 6, 7,
             6, 11, 7,    6,  0, 11,     6,  1, 0,    10, 1, 6,
            11,  0, 9,    2, 11,  9,     5,  2, 9,    11, 2, 7
        };

        for (auto& vertex : result.vertices) {
            auto pos  = vector_init(vertex.position.arr);
            auto norm = vector_normalize(pos);
            pos = norm * params.radius;

            vector_copy(pos,  vertex.position.arr);
            vector_copy(norm, vertex.normal.arr);

            float theta = atan2f(vertex.position.z(), vertex.position.x());
            if (theta < 0.0f)
                theta += _2PI;
            float phi = acosf(vertex.position.y() / params.radius);
            vertex.texture_coordinates = { theta / _2PI, phi / PI };
        }

        return result;
    }

    Mesh generate_figure(const FigureParams& params, const std::string& name) {
        if (std::holds_alternative<CuboidParams>(params)) {
            const auto& cube_params = std::get<CuboidParams>(params);
            return generate_cuboid(cube_params, name);
        }

        if (std::holds_alternative<SphereParams>(params)) {
            const auto& sphere_params = std::get<SphereParams>(params);
            return generate_sphere(sphere_params, name);
        }

        return {};
    }
}
