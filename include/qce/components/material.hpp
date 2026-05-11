// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <optional>

namespace QCE {
    struct MaterialComponent {
        std::optional<size_t> cpu_albedo_index    = std::nullopt;
        std::optional<size_t> cpu_normal_index    = std::nullopt;
        std::optional<size_t> cpu_metallic_index  = std::nullopt;
        std::optional<size_t> cpu_roughness_index = std::nullopt;
        std::optional<size_t> cpu_occlusion_index = std::nullopt;
        std::optional<size_t> cpu_emissive_index  = std::nullopt;

        std::optional<size_t> gpu_albedo_index    = std::nullopt;
    };
}