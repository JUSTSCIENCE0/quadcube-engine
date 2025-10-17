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

    ErrorCode ResourceManager::AddModel(
            const std::string& model_name,
            const std::string& mesh_name) {
        if (m_models.end() != m_models.find(model_name))
            return ErrorCode::E_RM_MODEL_ID_COLLISION;

        auto mesh_it = m_meshes.find(mesh_name);
        if (m_meshes.end() == mesh_it)
            return ErrorCode::E_RM_MESH_NOT_FOUND;

        m_models.emplace(model_name, std::make_shared<Model>(
            model_name, mesh_it->second));

        return ErrorCode::SUCCESS;
    }

    ErrorCode ResourceManager::AddFigure(const FigureParams& params, const std::string& mesh_name) {
        if (!mesh_name.empty()) {
            if (m_meshes.end() != m_meshes.find(mesh_name))
                return ErrorCode::SUCCESS;
        }

        auto mesh = generate_figure(params, mesh_name);
        assert(mesh);

        auto key = mesh->m_id;
        m_meshes.try_emplace(std::move(key), std::move(mesh));
        return ErrorCode::SUCCESS;
    }

    std::shared_ptr<Model> ResourceManager::GetModel(const std::string& name) {
        auto model_it = m_models.find(name);
        if (m_models.end() == model_it) {
            // TODO: log error and return error model
            return nullptr;
        }

        return model_it->second;
    }

    ErrorCode ResourceManager::AddEntity(
            const std::string& entity_name,
            const std::string& model_name,
            const Transform& start_transform) {
        [[maybe_unused]] auto entity = AddAndGetEntity(
            entity_name,
            model_name,
            start_transform);
        assert(entity);
        return ErrorCode::SUCCESS;
    }

    ErrorCode ResourceManager::AddShader(
            const std::string& shader_name,
        ShaderType shader_type) {
        const auto id = shader_name + "_" + get_shader_type_suffix(shader_type);
        if (m_shaders.end() != m_shaders.find(id))
            return ErrorCode::E_RM_SHADER_ALREADY_EXISTS;

        std::shared_ptr<Shader> shader = nullptr;
        try {
            shader = std::make_shared<Shader>(
                shader_name,
                shader_type,
                m_shaders_bytecode_directory,
                m_render_type
            );
        }
        catch (ErrorCodeException err) {
            return err.code_value();
        }

        m_shaders.emplace(id, std::move(shader));
        return ErrorCode::SUCCESS;
    }

    std::shared_ptr<Entity> ResourceManager::AddAndGetEntity(
            const std::string& entity_name,
            const std::string& model_name,
            const Transform& start_transform) {
        auto model = GetModel(model_name);
        assert(model);

        auto entity = std::make_shared<Entity>(
            entity_name, model, start_transform);

        auto key = entity->m_id;
        assert(m_entities.end() == m_entities.find(key));
        m_entities.try_emplace(key, entity);
        return entity;
    }

    std::shared_ptr<Shader> ResourceManager::GetShader(
            const std::string& shader_name,
            ShaderType shader_type) {
        const auto id = shader_name + "_" + get_shader_type_suffix(shader_type);
        auto shader_it = m_shaders.find(id);
        if (m_shaders.end() == shader_it) {
            return nullptr;
        }

        return shader_it->second;
    }
}
