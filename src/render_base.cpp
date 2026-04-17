// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifdef WIN32
#  include <qce/renders/render_dx12.hpp>
#endif

namespace QCE {
    ErrorCode RenderBase::UseShader(const std::string& name, ShaderType type) {
        if (ResourceManager::INVALID_RESOURCE_INDEX != m_shader_indeces[type])
            return ErrorCode::E_ENG_SHADER_ALREADY_SELECTED;
        auto shader_id = make_shader_id(name, type);
        auto shader_index = ResourceManager::Get().GetIndex<Shader>(shader_id);
        if (ResourceManager::INVALID_RESOURCE_INDEX == shader_index)
            return ErrorCode::E_ENG_SHADER_NOT_FOUND;

        m_shader_indeces[type] = shader_index;
        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderBase::UpdateCpuScene() {
        // reset
        m_scene_cpu.units.clear();
        m_scene_cpu.index_buffer.clear();
        m_scene_cpu.vertex_buffer.clear();
        m_scene_cpu.index_buffer_size = 0;
        m_scene_cpu.vertex_buffer_size = 0;

        auto entities = m_entities.QueryEntities<
            MeshComponent,
            TransformComponents,
            TransformMatrix>();
        for (const auto& entity_id : entities) {
            auto& mesh_comp = m_entities.GetComponent<MeshComponent>(entity_id);
            auto& mesh = ResourceManager::Get().Read<Mesh>(mesh_comp.index);
            mesh.render_unit_index.reset();
        }

        size_t unit_index = 0;

        for (const auto& entity_id : entities) {
            auto mesh_comp = m_entities.GetComponent<MeshComponent>(entity_id);
            auto& mesh = ResourceManager::Get().Read<Mesh>(mesh_comp.index);

            if (mesh.render_unit_index.has_value())
                continue;

            RenderSceneCPU::Unit unit{
                .indeces_count = uint32_t(mesh.indices.size()),
                .index_offset = uint32_t(m_scene_cpu.index_buffer.size()),
                .vertex_offset = uint32_t(m_scene_cpu.vertex_buffer.size())
            };

            m_scene_cpu.index_buffer.insert(
                m_scene_cpu.index_buffer.end(), mesh.indices.begin(), mesh.indices.end());
            m_scene_cpu.vertex_buffer.insert(
                m_scene_cpu.vertex_buffer.end(), mesh.vertices.begin(), mesh.vertices.end());
            m_scene_cpu.units.push_back(std::move(unit));

            mesh.render_unit_index = unit_index;
            unit_index++;
        }

        m_scene_cpu.vertex_buffer_size = uint32_t(m_scene_cpu.vertex_buffer.size()) * m_scene_cpu.VERTEX_STRIDE;
        m_scene_cpu.index_buffer_size = uint32_t(m_scene_cpu.index_buffer.size()) * sizeof(index_t);
        return ErrorCode::SUCCESS;
    }
}
