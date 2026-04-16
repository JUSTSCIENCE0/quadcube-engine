// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/systems/render_system.hpp>

#ifdef WIN32
#  include <qce/renders/render_dx12.hpp>
#endif

namespace QCE {
    RenderSystem::RenderSystem(Entities& entities) :
        m_entities(entities) {
        m_config.render_type = RenderType::E_RENDER_UNKNOWN;
    }

    void RenderSystem::SetWindow(void* window) {
        m_window = window;
    }

    ErrorCode RenderSystem::UseShader(const std::string& name, ShaderType type) {
        assert(m_render);
        return m_render->UseShader(name, type);
    }

    ErrorCode RenderSystem::Setup(const Config& config) {
        if (m_config.render_type != config.render_type) {
            m_render.reset();

            switch (config.render_type) {
            case RenderType::E_RENDER_DIRECTX12:
#ifdef WIN32
                assert(m_window);
                m_render = std::make_unique<RenderDX12>(m_entities, config, HWND(m_window));
#else
                assert(!"Current platform doesn't support DirectX 12");
                m_config.render_type = RenderType::E_RENDER_UNKNOWN;
#endif
                break;
            default:
                m_config.render_type = RenderType::E_RENDER_UNKNOWN;
                assert(!"Unsupported render type");
                break;
            }
        }

        assert(m_render);
        for (const auto& shader : config.used_shaders) {
            QCE_CRITICAL(
                ResourceManager::Get().AddShader(shader.shader_name, shader.shader_type));
            QCE_CRITICAL(UseShader(shader.shader_name, shader.shader_type));
        }

        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderSystem::Update() {
        assert(m_render);
        return m_render->Draw();
    }

    ErrorCode RenderSystem::UpdateScene() {
        assert(m_render);
        return m_render->UpdateScene();
    }
}
