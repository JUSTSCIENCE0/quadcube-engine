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
        auto entity = m_resource_manager.AddAndGetEntity(
            name, model, start_transform);
        assert(entity);
        assert(entity->m_name == name);

        auto uid = entity->m_uid;
        assert(m_entities[name].end() == m_entities[name].find(uid));
        m_entities[name].try_emplace(uid, std::move(entity));
        return ErrorCode::SUCCESS;
    }
}
