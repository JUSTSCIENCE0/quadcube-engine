// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>
#include <qce/objects/index.hpp>
#include <qce/objects/command.hpp>

#include <string>
#include <vector>
#include <utility>
#include <optional>

namespace QCE {
    struct MeshComponent {
        size_t index{};
    };

    struct DynamicMesh {
        size_t index{};
    };

    struct Mesh {
        Mesh() = default;
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(Mesh&&) noexcept = default;

        std::string          id{};
        std::vector<vertex>  vertices{};
        std::vector<index_t> indices{};
    };

    struct DeformatedMesh : CommandContext {
        DeformatedMesh(bool need_update_mesh = true) :
            CommandContext(CommandContextType::E_CCT_DEFORMATED_MESH),
            update_mesh(need_update_mesh) {}
        virtual ~DeformatedMesh() = default;

        const bool update_mesh;

        const Mesh* deformation_result = nullptr;
        size_t max_vertices_count = 0;
    };
}
