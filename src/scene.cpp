// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/objects/scene.hpp>
#include <qce/objects/resource_manager.hpp>

namespace QCE {
    ErrorCode add_entity_from_description(
            Entities& entities, const EntityDescription& desc) {
        auto& resources = ResourceManager::Get();

        std::optional<size_t> static_mesh_index;
        if (desc.static_mesh.has_value()) {
            const auto index = resources.GetIndex<Mesh>(desc.static_mesh.value());
            if (ResourceManager::INVALID_RESOURCE_INDEX == index)
                return ErrorCode::E_RM_MESH_NOT_FOUND;
            static_mesh_index = index;
        }

        std::optional<size_t> dynamic_mesh_base_index;
        std::optional<size_t> dynamic_mesh_deformated_index;
        if (desc.dynamic_mesh.has_value()) {
            dynamic_mesh_base_index = resources.GetIndex<Mesh>(desc.dynamic_mesh->base);
            if (ResourceManager::INVALID_RESOURCE_INDEX == dynamic_mesh_base_index)
                return ErrorCode::E_RM_MESH_NOT_FOUND;

            dynamic_mesh_deformated_index = resources.GetIndex<Mesh>(desc.dynamic_mesh->deformated);
            if (ResourceManager::INVALID_RESOURCE_INDEX == dynamic_mesh_deformated_index)
                return ErrorCode::E_RM_MESH_NOT_FOUND;
        }

        std::optional<size_t> material_index;
        if (desc.material.has_value()) {
            const auto index = resources.GetIndex<Material>(desc.material.value());
            if (ResourceManager::INVALID_RESOURCE_INDEX == index)
                return ErrorCode::E_RM_RESOURCE_NOT_FOUND;
            material_index = index;
        }

        std::optional<size_t> animation_index;
        if (desc.transform_animation.has_value()) {
            const auto index = resources.GetIndex<TransformAnimation>(
                desc.transform_animation->animation_name);
            if (ResourceManager::INVALID_RESOURCE_INDEX == index)
                return ErrorCode::E_RM_ANIMATION_NOT_FOUND;
            animation_index = index;
        }

        auto entity_id = entities.AddEntity();
        if (desc.name.has_value()) {
            QCE_CRITICAL(entities.AddComponent(entity_id, EntityName{ desc.name.value() }));
        }

        if (static_mesh_index.has_value()) {
            QCE_CRITICAL(entities.AddComponent(entity_id,
                StaticMesh{ .index = static_mesh_index.value() }));
        }

        if (dynamic_mesh_base_index.has_value()) {
            const auto& base_mesh = resources.Read<Mesh>(dynamic_mesh_base_index.value());
            QCE_CRITICAL(entities.AddComponent(entity_id, DynamicMesh{
                .base_mesh_index = dynamic_mesh_base_index.value(),
                .max_vertices_count = static_cast<uint32_t>(base_mesh.vertices.size()),
                .max_indeces_count = static_cast<uint32_t>(base_mesh.indices.size()),
                .deformated_mesh_index = dynamic_mesh_deformated_index.value()
            }));
        }

        if (material_index.has_value()) {
            QCE_CRITICAL(entities.AddComponent(entity_id,
                MaterialComponent{ .index = material_index.value() }));
        }

        if (desc.directional_light.has_value()) {
            QCE_CRITICAL(entities.AddComponent(entity_id, desc.directional_light.value()));
        }
        if (desc.point_light.has_value()) {
            QCE_CRITICAL(entities.AddComponent(entity_id, desc.point_light.value()));
        }
        if (desc.spot_light.has_value()) {
            QCE_CRITICAL(entities.AddComponent(entity_id, desc.spot_light.value()));
        }

        QCE_CRITICAL(entities.AddComponent(entity_id,
            desc.transform.value_or(TransformComponents{})));
        QCE_CRITICAL(entities.AddComponent(entity_id, TransformMatrix{}));

        if (animation_index.has_value()) {
            QCE_CRITICAL(entities.AddComponent(entity_id, TransformAnimationComponent{
                .index = animation_index.value(),
                .is_looped = desc.transform_animation->is_looped
            }));
        }

        return ErrorCode::SUCCESS;
    }
}
