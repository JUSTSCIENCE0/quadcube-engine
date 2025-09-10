// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/error_codes.hpp>
#include <qce/objects/model.hpp>
#include <qce/objects/figures.hpp>

#include <unordered_map>
#include <concepts>
#include <type_traits>

namespace QCE {
    class ResourseManager final {
    public:
        ResourseManager() = default;
        ResourseManager(const ResourseManager&) = delete;
        ResourseManager(ResourseManager&&) = delete;
        ResourseManager& operator=(const ResourseManager&) = delete;
        ResourseManager& operator=(ResourseManager&&) = delete;

        template<typename SharedMesh>
        requires std::same_as<std::remove_cvref_t<SharedMesh>, std::shared_ptr<Mesh>>
        ErrorCode AddMesh(SharedMesh&& mesh) {
            if (m_meshes.end() != m_meshes.find(mesh->m_id))
                return ErrorCode::E_RM_MESH_ID_COLLISION;

            m_meshes.emplace(mesh->m_id, std::forward<SharedMesh>(mesh));
            return ErrorCode::SUCCESS;
        }

        ErrorCode AddModel(
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

        // TODO:
        // Mesh
        //  Generate Mesh (Figure)
        //  Load Mesh (from file)
        // Model
        //  Load Model (from file)
        //  Add Model (as shared_ptr)

    private:
        template <typename T>
        using Storage = std::unordered_map<std::string, std::shared_ptr<T>>;

        Storage<Mesh>  m_meshes{};
        Storage<Model> m_models{};
    };
}
