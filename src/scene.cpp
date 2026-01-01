// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/objects/scene.hpp>

namespace QCE {
    ErrorCode Scene::AddEntity(
            const std::string& name,
            const std::string& mesh,
            const Transform& start_transform) {
        if (m_entities.end() != m_entities.find(name))
            return ErrorCode::E_ENG_ENTITY_ALREADY_EXISTS;

        auto entity = ResourceManager::Get().AddAndGetEntity(
            name, mesh, start_transform);
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
            const float3d& up,
            CameraOperatorType operator_type) {
        bool is_LH_system = (m_render_type == RenderType::E_RENDER_DIRECTX12);
        auto camera = std::make_shared<Camera>(is_LH_system);

        if (aspect <= 0.0f)
            return ErrorCode::E_ENG_WRONG_ASPECT;
        if (fov_rad < deg_to_rad(MIN_FOV_DEG))
            return ErrorCode::E_ENG_WRONG_FOV;
        if (zfar <= znear)
            return ErrorCode::E_ENG_WRONG_ZLIMITS;

        camera->SetAspect(aspect);
        camera->SetFoV(fov_rad);
        camera->SetZLimits(znear, zfar);
        camera->SetPosition(position);
        camera->SetTarget(target);
        camera->SetUp(up);

        std::unique_ptr<CameraOperator> camera_operator = nullptr;
        switch (operator_type) {
        case CameraOperatorType::E_CAMERA_OPERATOR_FIRST_PERSON:
            camera_operator = std::make_unique<FirstPersonCameraOperator>(
                "CameraOperator" + std::to_string(m_cameras.size()), camera);
            break;
        default:
            break;
        }

        m_cameras.emplace_back(CameraUnit{ std::move(camera), std::move(camera_operator) });
        return ErrorCode::SUCCESS;
    }

    ErrorCode Scene::UseShader(const std::string& name, ShaderType type) {
        if (ResourceManager::INVALID_RESOURCE_INDEX != m_shaders[type])
            return ErrorCode::E_ENG_SHADER_ALREADY_SELECTED;
        auto shader_id = make_shader_id(name, type);
        auto shader_index = ResourceManager::Get().GetIndex<Shader>(shader_id);
        if (ResourceManager::INVALID_RESOURCE_INDEX == shader_index)
            return ErrorCode::E_ENG_SHADER_NOT_FOUND;

        m_shaders[type] = shader_index;
        return ErrorCode::SUCCESS;
    }

    ErrorCode Scene::Update() {
        // update entities
        // TODO

        // update cameras
        for (auto & camera_unit : m_cameras) {
            if (camera_unit.camera_operator) {
                QCE_CRITICAL(camera_unit.camera_operator->Update());
            }
        }

        return ErrorCode::SUCCESS;
    }
}
