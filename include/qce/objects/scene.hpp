// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/entity.hpp>
#include <qce/objects/resource_manager.hpp>

#include <list>

namespace QCE {
    class Scene {
    public:
        using Entities = std::unordered_map<
            std::string/*name*/,
            std::unordered_map<
                uid_t,
                std::shared_ptr<Entity>
            >
        >;
        struct Description {
            Entities& entities;
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
            const transform& start_transform = {});

        Description GetDescription() {
            return {
                .entities = m_entities
            };
        }

    private:
        ResourceManager& m_resource_manager;

        Entities m_entities{};
    };
}
