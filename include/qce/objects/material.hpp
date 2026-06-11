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
        std::optional<size_t> albedo_texture    = std::nullopt;
        std::optional<size_t> normal_texture    = std::nullopt;
        std::optional<size_t> metallic_texture  = std::nullopt;
        std::optional<size_t> roughness_texture = std::nullopt;
        std::optional<size_t> occlusion_texture = std::nullopt;
        std::optional<size_t> emissive_texture  = std::nullopt;
    };
}