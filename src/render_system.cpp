// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/systems/render_system.hpp>
#include <qce/ancillary/directories.hpp>
#include <qce/loaders/read.hpp>

#include <format>

#ifdef WIN32
#  include <qce/renders/render_dx12.hpp>
#endif

namespace QCE {
    RenderSystem::RenderSystem(Entities& entities) :
        m_entities(entities) {}

    void RenderSystem::SetWindow(void* window) {
        m_window = window;
    }

    ErrorCode RenderSystem::UseShader(const std::string& name, ShaderType type) {
        assert(m_render);
        return m_render->UseShader(name, type);
    }

    ErrorCode RenderSystem::Setup() {
        const auto CONFIGS_DIR = QCE::get_configs_directory();
        const auto render_config_file = std::format("{}/{}.{}", CONFIGS_DIR.string(), "render_system", SYSTEM_CONFIG_CONTAINER);

        RenderConfig config{};
        QCE_CRITICAL(read_from_file(render_config_file, config));

        const auto common_config_file = std::format("{}/{}.{}", CONFIGS_DIR.string(), "common_config", SYSTEM_CONFIG_CONTAINER);
        CommonConfig common_config{};
        QCE_CRITICAL(read_from_file(common_config_file, common_config));

        if (m_render_type != common_config.render_type) {
            m_render.reset();

            switch (common_config.render_type) {
            case RenderType::E_RENDER_DIRECTX12:
#ifdef WIN32
                assert(m_window);
                m_render = std::make_unique<RenderDX12>(m_entities, config, HWND(m_window));
                m_render_type = RenderType::E_RENDER_DIRECTX12;
#else
                assert(!"Current platform doesn't support DirectX 12");
                m_render_type = RenderType::E_RENDER_UNKNOWN;
#endif
                break;
            default:
                m_render_type = RenderType::E_RENDER_UNKNOWN;
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
