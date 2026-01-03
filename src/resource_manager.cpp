// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/objects/resource_manager.hpp>

namespace QCE {
    std::filesystem::path ResourceManager::GetResourcesDirectory() {
        std::filesystem::path result = CU::get_current_module_directory();
        result.append(RESOURCES_DIRECTORY);

        if (!std::filesystem::exists(result) ||
            !std::filesystem::is_directory(result))
            throw ErrorCodeException(E_RM_BAD_RESOURCES_DIRECTORY);

        return result;
    }

    void ResourceManager::RegisterDefaultCommands() {
        Add(Command{
            "Exit", std::make_shared<ExitCommand>()
        });
    }

    ErrorCode ResourceManager::AddFigure(const FigureParams& params, const std::string& mesh_name) {
        if (!mesh_name.empty()) {
            if (Exists<Mesh>(mesh_name))
                return ErrorCode::E_RM_RESOURCE_ALREADY_EXISTS;
        }

        auto mesh = generate_figure(params, mesh_name);
        assert(
            !mesh.id.empty() &&
            !mesh.indices.empty() &&
            !mesh.vertices.empty());

        return Add(mesh);
    }

    ErrorCode ResourceManager::AddEntity(
            const std::string& entity_name,
            const std::string& mesh_name,
            const Transform& start_transform) {
        [[maybe_unused]] auto entity = AddAndGetEntity(
            entity_name,
            mesh_name,
            start_transform);
        assert(entity);
        return ErrorCode::SUCCESS;
    }

    ErrorCode ResourceManager::AddShader(
            const std::string& shader_name,
            ShaderType shader_type) {
        Shader shader{};
        QCE_CRITICAL(
            load_shader(
                shader_name,
                shader_type,
                m_shaders_bytecode_directory,
                m_render_type,
                shader
            )
        );

        return Add(shader);
    }

    std::shared_ptr<Entity> ResourceManager::AddAndGetEntity(
            const std::string& entity_name,
            const std::string& mesh_name,
            const Transform& start_transform) {
        auto mesh_index = GetIndex<Mesh>(mesh_name);
        assert(INVALID_RESOURCE_INDEX != mesh_index);

        auto entity = std::make_shared<Entity>(
            entity_name, mesh_index, start_transform);

        auto key = entity->m_id;
        assert(m_entities.end() == m_entities.find(key));
        m_entities.try_emplace(key, entity);
        return entity;
    }
}
