// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/mesh.hpp>
#include <qce/components/transform.hpp>

#include <cu/id-utils.hpp>

namespace QCE {
    using uid_t = CU::uid_t;
    using UidGenerator = CU::UidGenerator;

    struct Entity {
        explicit Entity(
                std::string name,
                size_t mesh_index,
                Transform start_transform) :
            m_name(std::move(name)),
            m_uid(CU::get_uid()),
            m_id(CU::concat_with_uid(m_name, m_uid)),
            m_mesh_index(mesh_index),
            m_transform(std::move(start_transform)) {}
        Entity(const Entity&) = delete;
        Entity(Entity&&) = delete;
        Entity& operator=(const Entity&) = delete;
        Entity& operator=(Entity&&) = delete;

        const std::string m_name;
        const CU::uid_t m_uid;
        const std::string m_id;
        size_t m_mesh_index = std::numeric_limits<size_t>::max();
        Transform m_transform{};
    };
}
