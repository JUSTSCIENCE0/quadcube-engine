// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/entity.hpp>
#include <qce/objects/resource_manager.hpp>

#include <list>
#include <array>

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
        struct Description {
            Entities& entities;
            Shaders& shaders;
        };

        explicit Scene(ResourceManager& rm) :
            m_resource_manager(rm) {}
        Scene(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene& operator=(Scene&&) = delete;

        ErrorCode AddEntity(
            const std::string& name,
            const std::string& model,
            const Transform& start_transform = {});

        // TODO:
        // ErrorCode AddEntityDuplicate(
        //    const std::string& name,
        //    const transform& start_transform = {});

        ErrorCode UseShader(const std::string& name, ShaderType type);

        Description GetDescription() {
            return {
                .entities = m_entities,
                .shaders = m_shaders
            };
        }

    private:
        ResourceManager& m_resource_manager;

        Entities m_entities{};
        Shaders m_shaders{};
    };
}
