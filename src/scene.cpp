// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/objects/scene.hpp>

namespace QCE {
    ErrorCode Scene::AddEntity(
            const std::string& name,
            const std::string& model,
            const transform& start_transform) {
        if (m_entities.end() != m_entities.find(name))
            return ErrorCode::E_ENG_ENTITY_ALREADY_EXISTS;

        auto entity = m_resource_manager.AddAndGetEntity(
            name, model, start_transform);
        assert(entity);
        assert(entity->m_name == name);

        m_entities[name].push_back(std::move(entity));
        return ErrorCode::SUCCESS;
    }
}
