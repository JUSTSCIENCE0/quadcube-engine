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
            {
                .position = { -l2, -w2, -h2 },
                .color = WHITE,
                .texture_coordinates = { 0.0f, 1.0f }
            },
            {
                .position = {  l2, -w2, -h2 },
                .color = BLACK,
                .texture_coordinates = { 1.0f, 1.0f }
            },
            {
                .position = {  l2, -w2,  h2 },
                .color = RED,
                .texture_coordinates = { 1.0f, 1.0f }
            },
            {
                .position = { -l2, -w2,  h2 },
                .color = GREEN,
                .texture_coordinates = { 0.0f, 1.0f }
            },
            {
                .position = { -l2,  w2, -h2 },
                .color = BLUE,
                .texture_coordinates = { 0.0f, 0.0f }
            },
            {
                .position = {  l2,  w2, -h2 },
                .color = YELLOW,
                .texture_coordinates = { 1.0f, 1.0f }
            },
            {
                .position = {  l2,  w2,  h2 },
                .color = CYAN,
                .texture_coordinates = { 1.0f, 1.0f }
            },
            {
                .position = { -l2,  w2,  h2 },
                .color = MAGENTA,
                .texture_coordinates = { 0.0f, 0.0f }
            },
        };
        result.indices = std::vector<index_t>{
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
