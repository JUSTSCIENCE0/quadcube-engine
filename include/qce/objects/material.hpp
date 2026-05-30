// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>

#include <string>
#include <optional>

namespace QCE {
    struct MaterialComponent {
        size_t index = 0;
    };

    struct Material {
        Material() = default;
        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;
        Material(Material&&) = default;
        Material& operator=(Material&&) noexcept = default;

        std::string id{};

        /// base
        color_rgba albedo_color = WHITE;
        float3d    fresnel = { 0.01f, 0.01f, 0.01f };
        float      shininess = 0.75f;

        /// textures
        //  cpu resource indices
        std::optional<size_t> cpu_albedo_index = std::nullopt;
        std::optional<size_t> cpu_normal_index = std::nullopt;
        std::optional<size_t> cpu_metallic_index = std::nullopt;
        std::optional<size_t> cpu_roughness_index = std::nullopt;
        std::optional<size_t> cpu_occlusion_index = std::nullopt;
        std::optional<size_t> cpu_emissive_index = std::nullopt;
        //  gpu resource indices
        std::optional<size_t> gpu_albedo_index = std::nullopt;
    };
}