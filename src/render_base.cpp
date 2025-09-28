// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifdef WIN32
#  include <qce/renders/render_dx12.hpp>
#endif

namespace QCE {
    std::shared_ptr<RenderBase> GetRender(
            RenderConfig config, void* window, void* app) {
        static RenderType current_type = RenderType::E_RenderType_UNKNOWN;
        static std::shared_ptr<RenderBase> result = nullptr;

        if (current_type != config.render_type) {
            current_type = config.render_type;
            result.reset();

            switch (config.render_type) {
            case RenderType::E_RENDER_DIRECTX12:
#ifdef WIN32
                // unfortunatly we can't use std::make_shared for private constructor
                result = std::shared_ptr<RenderBase>(new RenderDX12(config, HWND(window)));
#else
                assert(!"Current platform doesn't support DirectX 12");
                current_type = RenderType::E_RenderType_UNKNOWN;
#endif
                break;
            default:
                current_type = RenderType::E_RenderType_UNKNOWN;
                break;
            }
        }

        return result;
    }

    ErrorCode RenderBase::UpdateCpuScene() {
        m_scene_cpu.units.clear();
        m_scene_cpu.index_buffer.clear();
        m_scene_cpu.vertex_buffer.clear();

        auto scene = m_current_scene->GetDescription();
        for (const auto& [name, entities_group] : scene.entities) {
            const auto& entity = entities_group.begin()->second;
            const auto& mesh = entity->m_model->m_mesh;

            if (m_scene_cpu.units.end() != m_scene_cpu.units.find(mesh->m_id))
                continue;

            RenderUnit unit{
                .indeces_count = uint32_t(mesh->m_indices.size()),
                .index_offset = uint32_t(m_scene_cpu.index_buffer.size()),
                .vertex_offset = uint32_t(m_scene_cpu.vertex_buffer.size())
            };

            m_scene_cpu.index_buffer.insert(
                m_scene_cpu.index_buffer.end(), mesh->m_indices.begin(), mesh->m_indices.end());
            m_scene_cpu.vertex_buffer.insert(
                m_scene_cpu.vertex_buffer.end(), mesh->m_vertices.begin(), mesh->m_vertices.end());
            m_scene_cpu.units.emplace(mesh->m_id, std::move(unit));
        }

        return ErrorCode::SUCCESS;
    }
}
