// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/objects/figures.hpp>

#include <iomanip>
#include <format>

namespace QCE {
    static inline vertex middle_vertex(const vertex& v1, const vertex& v2) {
        auto p0 = vector_init(v1.position.arr);
        auto p1 = vector_init(v2.position.arr);
        auto n0 = vector_init(v1.normal.arr);
        auto n1 = vector_init(v2.normal.arr);

        auto pos = (p0 + p1) * 0.5f;
        auto normal = vector_normalize((n0 + n1) * 0.5f);

        vertex result{};
        vector_copy(pos, result.position.arr);
        vector_copy(normal, result.normal.arr);
        result.texture_coordinates.x() = (v1.texture_coordinates.x() + v2.texture_coordinates.x()) * 0.5f;
        result.texture_coordinates.y() = (v1.texture_coordinates.y() + v2.texture_coordinates.y()) * 0.5f;
        return result;
    }

    static inline void subdivide_mesh(Mesh& mesh, int subdivisions) {
        // TODO: Hard edges mode

        if (subdivisions < 1) {
            return;
        }
        assert(mesh.indices.size() % 3 == 0);

        mesh.vertices.reserve(mesh.vertices.size() * 2 * subdivisions);
        mesh.indices.reserve(mesh.indices.size() * 4 * subdivisions);

        std::vector<vertex> vertices;
        vertices.reserve(mesh.vertices.capacity());
        std::vector<index_t> indices;
        indices.reserve(mesh.indices.capacity());

        for (int subdivision = 0; subdivision < subdivisions; subdivision++) {
            auto num_triangles = static_cast<index_t>(mesh.indices.size() / 3 );
            for (index_t i = 0; i < num_triangles; i++) {
                auto i0 = mesh.indices[i * 3 + 0];
                auto i1 = mesh.indices[i * 3 + 1];
                auto i2 = mesh.indices[i * 3 + 2];
                auto v0 = mesh.vertices[i0];
                auto v1 = mesh.vertices[i1];
                auto v2 = mesh.vertices[i2];

                auto m0 = middle_vertex(v0, v1);
                auto m1 = middle_vertex(v1, v2);
                auto m2 = middle_vertex(v2, v0);

                vertices.push_back(v0);
                vertices.push_back(v1);
                vertices.push_back(v2);
                vertices.push_back(m0);
                vertices.push_back(m1);
                vertices.push_back(m2);

                indices.push_back(i * 6 + 0);
                indices.push_back(i * 6 + 3);
                indices.push_back(i * 6 + 5);

                indices.push_back(i * 6 + 3);
                indices.push_back(i * 6 + 1);
                indices.push_back(i * 6 + 4);

                indices.push_back(i * 6 + 5);
                indices.push_back(i * 6 + 4);
                indices.push_back(i * 6 + 2);

                indices.push_back(i * 6 + 3);
                indices.push_back(i * 6 + 4);
                indices.push_back(i * 6 + 5);
            }

            mesh.vertices.swap(vertices);
            mesh.indices.swap(indices);
            vertices.clear();
            indices.clear();
        }
    }

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

        subdivide_mesh(result, params.subdivisions);

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
