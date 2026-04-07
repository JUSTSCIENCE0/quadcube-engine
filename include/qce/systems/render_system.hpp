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

        explicit RenderSystem(Entities& entities) :
            m_entities(entities) {
            m_config.render_type = RenderType::E_RENDER_UNKNOWN;
        }

        void SetWindow(void* window) {
            m_window = window;
        }

        ErrorCode Setup(const Config& config);

        ErrorCode Update() {
            assert(m_render);
            return m_render->Draw();
        }

        ErrorCode UpdateScene() {
            assert(m_render);
            return m_render->UpdateScene();
        }

        ErrorCode UseShader(const std::string& name, ShaderType type) {
            assert(m_render);
            return m_render->UseShader(name, type);
        }

    private:
        Entities& m_entities;

        Config m_config{};
        std::unique_ptr<RenderBase> m_render = nullptr;

        void* m_window = nullptr;
    };
}
