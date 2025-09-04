// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/model.hpp>

namespace QCE {
    class Entity {


    private:
        std::shared_ptr<Model> m_model = nullptr;

        transform m_transform{};
    };
}
