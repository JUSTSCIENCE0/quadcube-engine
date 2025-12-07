// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/mesh.hpp>
#include <qce/objects/shader.hpp>

#include <memory>

namespace QCE {
    struct Model {
        explicit Model(
                std::string           name,
                std::shared_ptr<Mesh> mesh) :
            m_id(std::move(name)),
            m_mesh(std::move(mesh)) {}

        Model(const Model&) = delete;
        Model(Model&&) = default;
        Model& operator=(const Model&) = delete;
        Model& operator=(Model&&) = default;

        const std::string     m_id{};
        std::shared_ptr<Mesh> m_mesh{};
    };
}
