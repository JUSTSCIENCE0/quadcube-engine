// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/entity.hpp>
#include <qce/objects/resource_manager.hpp>

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
        struct Description {
            Entities& entities;
        };

        explicit Scene(RenderType render_type):
            m_render_type(render_type) {}
        Scene(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene& operator=(Scene&&) = delete;

        ErrorCode AddEntity(
            const std::string& name,
            const std::string& mesh,
            const Transform& start_transform = {});

        // TODO:
        // ErrorCode AddEntityDuplicate(
        //    const std::string& name,
        //    const transform& start_transform = {});

        Description GetDescription() {
            return {
                .entities = m_entities
            };
        }

        size_t GetEntitiesCount() const noexcept { return m_entities_count; }
    private:
        static constexpr float MIN_FOV_DEG = 30.0f;

        RenderType m_render_type = DEFAULT_RENDER_TYPE;

        Entities m_entities{};
        size_t   m_entities_count = 0;
    };
}
