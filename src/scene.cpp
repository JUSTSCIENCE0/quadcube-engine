// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/objects/scene.hpp>

namespace QCE {
    ErrorCode Scene::AddEntity(
            const std::string& name,
            const std::string& model,
            const Transform& start_transform) {
        if (m_entities.end() != m_entities.find(name))
            return ErrorCode::E_ENG_ENTITY_ALREADY_EXISTS;

        auto entity = ResourceManager::Get().AddAndGetEntity(
            name, model, start_transform);
        assert(entity);
        assert(entity->m_name == name);

        m_entities[name].push_back(std::move(entity));
        m_entities_count++;
        return ErrorCode::SUCCESS;
    }

    ErrorCode Scene::AddCamera(
            float aspect,
            float fov_rad,
            float znear,
            float zfar,
            const float3d& position,
            const float3d& target,
            const float3d& up) {
        Camera camera{ m_render_type == RenderType::E_RENDER_DIRECTX12 };

        if (aspect <= 0.0f)
            return ErrorCode::E_ENG_WRONG_ASPECT;
        if (fov_rad < deg_to_rad(MIN_FOV_DEG))
            return ErrorCode::E_ENG_WRONG_FOV;
        if (zfar <= znear)
            return ErrorCode::E_ENG_WRONG_ZLIMITS;

        camera.SetAspect(aspect);
        camera.SetFoV(fov_rad);
        camera.SetZLimits(znear, zfar);
        camera.SetPosition(position);
        camera.SetTarget(target);
        camera.SetUp(up);

        m_cameras.push_back(std::move(camera));
        return ErrorCode::SUCCESS;
    }

    ErrorCode Scene::UseShader(const std::string& name, ShaderType type) {
        if (m_shaders[type])
            return ErrorCode::E_ENG_SHADER_ALREADY_SELECTED;
        auto shader = ResourceManager::Get().GetShader(name, type);
        if (!shader)
            return ErrorCode::E_ENG_SHADER_NOT_FOUND;

        m_shaders[type] = std::move(shader);
        return ErrorCode::SUCCESS;
    }
}
