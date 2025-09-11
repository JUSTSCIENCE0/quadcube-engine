// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/model.hpp>

namespace QCE {
    class Entity {
        explicit Entity(std::string name, std::shared_ptr<Model> model) :
            m_id(std::move(name)),
            m_model(std::move(model)) {}
        Entity(const Entity&) = delete;
        Entity(Entity&&) = delete;
        Entity& operator=(const Entity&) = delete;
        Entity& operator=(Entity&&) = delete;

    private:
        std::string m_id;
        std::shared_ptr<Model> m_model = nullptr;
        transform m_transform{};
    };
}
