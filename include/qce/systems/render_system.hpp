// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/ecs.hpp>
#include <qce/renders/render.hpp>

namespace QCE {
    class RenderSystem {
    public:
        using Config = RenderConfig;

        explicit RenderSystem(Entities& entities);

        void SetWindow(void* window);
        ErrorCode Setup();
        ErrorCode UseShader(const std::string& name, ShaderType type);

        ErrorCode Update();
        ErrorCode UpdateScene();

    private:
        Entities& m_entities;

        Config m_config{};
        std::unique_ptr<RenderBase> m_render = nullptr;

        void* m_window = nullptr;
    };
}
