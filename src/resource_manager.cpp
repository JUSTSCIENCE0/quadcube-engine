// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/objects/resource_manager.hpp>

namespace QCE {
    ErrorCode ResourceManager::AddModel(
            const std::string& model_name,
            const std::string& mesh_name) {
        if (m_models.end() != m_models.find(model_name))
            return ErrorCode::E_RM_MODEL_ID_COLLISION;

        auto mesh_it = m_meshes.find(mesh_name);
        if (m_meshes.end() == mesh_it)
            return ErrorCode::E_RM_MESH_NOT_FOUND;

        m_models[model_name] = std::make_shared<Model>(
            model_name, mesh_it->second);

        return ErrorCode::SUCCESS;
    }

    ErrorCode ResourceManager::AddFigure(const FigureParams& params, const std::string& mesh_name) {
        if (!mesh_name.empty()) {
            if (m_meshes.end() != m_meshes.find(mesh_name))
                return ErrorCode::SUCCESS;
        }

        auto mesh = generate_figure(params, mesh_name);
        if (m_meshes.end() != m_meshes.find(mesh->m_id))
            return ErrorCode::SUCCESS;

        auto key = mesh->m_id;
        m_meshes.emplace(std::move(key), std::move(mesh));
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
}
