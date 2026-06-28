// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifdef WIN32
#  include <qce/renders/render_dx12.hpp>
#endif

namespace QCE {
    ErrorCode RenderBase::UseShader(const std::string& name, ShaderType type) {
        if (ResourceManager::INVALID_RESOURCE_INDEX != m_shader_map[type])
            return ErrorCode::E_ENG_SHADER_ALREADY_SELECTED;
        auto shader_id = make_shader_id(name, type);
        auto shader_index = ResourceManager::Get().GetIndex<Shader>(shader_id);
        if (ResourceManager::INVALID_RESOURCE_INDEX == shader_index)
            return ErrorCode::E_ENG_SHADER_NOT_FOUND;

        m_shader_map[type] = shader_index;
        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderBase::UpdateScene() {
        // reset
        m_scene_materials.components.clear();
        m_scene_materials.dirty_frames.clear();
        m_material_buffer_map.clear();

        // update
        UpdateStaticGeometry();
        UpdateDynamicGeometry();

        return ErrorCode::SUCCESS;
    }

    void RenderBase::UpdateStaticGeometry() {
        // reset
        m_scene_static_geometry.units.clear();
        m_scene_static_geometry.index_buffer.clear();
        m_scene_static_geometry.vertex_buffer.clear();
        m_scene_static_geometry.index_buffer_size = 0;
        m_scene_static_geometry.vertex_buffer_size = 0;
        m_static_geometry_unit_map.clear();

        auto entities = m_entities.QueryEntities<
            MeshComponent,
            TransformComponents,
            TransformMatrix,
            MaterialComponent>();

        size_t unit_index = 0;
        for (const auto& entity_id : entities) {
            auto& mesh_comp = m_entities.GetComponent<MeshComponent>(entity_id);
            if (m_static_geometry_unit_map.exists(mesh_comp.index))
                continue;

            auto& mesh = ResourceManager::Get().Read<Mesh>(mesh_comp.index);

            SceneGeometry::Unit unit{
                .indeces_count = uint32_t(mesh.indices.size()),
                .index_offset = uint32_t(m_scene_static_geometry.index_buffer.size()),
                .vertex_offset = uint32_t(m_scene_static_geometry.vertex_buffer.size())
            };

            m_scene_static_geometry.index_buffer.insert(
                m_scene_static_geometry.index_buffer.end(), mesh.indices.begin(), mesh.indices.end());
            m_scene_static_geometry.vertex_buffer.insert(
                m_scene_static_geometry.vertex_buffer.end(), mesh.vertices.begin(), mesh.vertices.end());
            m_scene_static_geometry.units.emplace_back(std::move(unit));

            m_static_geometry_unit_map.add(mesh_comp.index, unit_index);
            unit_index++;
        }

        m_scene_static_geometry.vertex_buffer_size = uint32_t(m_scene_static_geometry.vertex_buffer.size()) * m_scene_static_geometry.VERTEX_STRIDE;
        m_scene_static_geometry.index_buffer_size = uint32_t(m_scene_static_geometry.index_buffer.size()) * sizeof(index_t);

        UpdateSceneMaterials(entities);
    }

    void RenderBase::UpdateDynamicGeometry() {
        m_scene_dynamic_geometry.units.clear();
        m_scene_dynamic_geometry.index_buffer_size = 0;
        m_scene_dynamic_geometry.vertex_buffer_size = 0;

        auto entities = m_entities.QueryEntities<
            DynamicMesh,
            TransformComponents,
            TransformMatrix,
            MaterialComponent>();
        for (const auto& entity_id : entities) {
            auto& dynamic_mesh = m_entities.GetComponent<DynamicMesh>(entity_id);
            auto& deformator = ResourceManager::Get().Read<Command>(dynamic_mesh.index);

            DeformatedMesh deformated_mesh{/*need_update_mesh*/false};
            deformator.command->Execute(&deformated_mesh);

            assert(deformated_mesh.max_vertices_count);
            assert(deformated_mesh.max_indeces_count);

            SceneGeometry::Unit unit{
                .indeces_count = uint32_t(deformated_mesh.max_indeces_count),
                .index_offset =  uint32_t(m_scene_dynamic_geometry.index_buffer_size),
                .vertex_offset = uint32_t(m_scene_dynamic_geometry.vertex_buffer_size)
            };

            m_scene_dynamic_geometry.vertex_buffer_size += deformated_mesh.max_vertices_count;
            m_scene_dynamic_geometry.index_buffer_size += deformated_mesh.max_indeces_count;
            m_scene_dynamic_geometry.units.push_back(unit);
        }

        m_scene_dynamic_geometry.vertex_buffer_size *= m_scene_static_geometry.VERTEX_STRIDE;
        m_scene_dynamic_geometry.index_buffer_size *= sizeof(index_t);

        UpdateSceneMaterials(entities);
    }

    void RenderBase::UpdateSceneMaterials(const std::set<CU::id_t>& entities) {
        size_t material_index = 0;
        for (const auto& entity_id : entities) {
            auto& material_comp = m_entities.GetComponent<MaterialComponent>(entity_id);
            if (m_material_buffer_map.exists(material_comp.index))
                continue;

            m_scene_materials.components.push_back(material_comp.index);
            m_material_buffer_map.add(material_comp.index, material_index);
            material_index++;
        }

        m_scene_materials.dirty_frames.resize(
            m_scene_materials.components.size(), FRAME_RESOURCE_COUNT);
    }
}
