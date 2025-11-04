// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/entity.hpp>
#include <qce/objects/resource_manager.hpp>
#include <qce/objects/camera.hpp>

#include <list>
#include <array>
#include <vector>

namespace QCE {
    class Scene {
    public:
        using Entities = std::unordered_map<
            std::string/*name*/,
            std::vector<std::shared_ptr<Entity>>
        >;
        using Shaders = std::array<
            std::shared_ptr<Shader>,
            ShaderType::E_SHADERS_TYPE_COUNT>;
        using Cameras = std::vector<Camera>;
        struct Description {
            Entities& entities;
            Shaders& shaders;
            Cameras& cameras;
        };

        explicit Scene(
                RenderType render_type,
                ResourceManager& rm) :
            m_render_type(render_type),
            m_resource_manager(rm) {}
        Scene(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene& operator=(Scene&&) = delete;

        ErrorCode AddEntity(
            const std::string& name,
            const std::string& model,
            const Transform& start_transform = {});

        ErrorCode AddCamera(
            float aspect = (16.0f / 9.0f),
            float fov_rad = deg_to_rad(60),
            float znear = 1.0f,
            float zfar = 1000.0f,
            const float3d& position = { 2.0f, 2.0f, -2.0f },
            const float3d& target = { 0.0f, 0.0f, 0.0f },
            const float3d& up = { 0.0f, 1.0f, 0.0f }
        );

        // TODO:
        // ErrorCode AddEntityDuplicate(
        //    const std::string& name,
        //    const transform& start_transform = {});

        ErrorCode UseShader(const std::string& name, ShaderType type);

        Description GetDescription() {
            return {
                .entities = m_entities,
                .shaders = m_shaders,
                .cameras = m_cameras
            };
        }

    private:
        static constexpr float MIN_FOV_DEG = 30.0f;

        RenderType m_render_type = DEFAULT_RENDER_TYPE;

        ResourceManager& m_resource_manager;

        Entities m_entities{};
        Shaders  m_shaders{};
        Cameras  m_cameras{};
    };
}
