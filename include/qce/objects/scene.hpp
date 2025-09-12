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
        explicit Scene(ResourceManager& rm) :
            m_resource_manager(rm) {}
        Scene(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene& operator=(Scene&&) = delete;

        ErrorCode AddEntity(const std::string name, const std::string model);

    private:
        ResourceManager& m_resource_manager;

        std::list<std::shared_ptr<Entity>> m_entities{};
    };
}
