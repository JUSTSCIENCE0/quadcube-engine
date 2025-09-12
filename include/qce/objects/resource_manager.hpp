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
    class ResourceManager final {
    public:
        ResourceManager() = default;
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;
        ResourceManager& operator=(ResourceManager&&) = delete;

        template<typename SharedMesh>
        requires std::same_as<std::remove_cvref_t<SharedMesh>, std::shared_ptr<Mesh>>
        ErrorCode AddMesh(SharedMesh&& mesh) {
            if (m_meshes.end() != m_meshes.find(mesh->m_id))
                return ErrorCode::E_RM_MESH_ID_COLLISION;

            auto key = mesh->m_id;
            m_meshes.emplace(std::move(key), std::forward<SharedMesh>(mesh));
            return ErrorCode::SUCCESS;
        }
        ErrorCode AddFigure(const FigureParams& params, const std::string& mesh_name="");
        ErrorCode AddModel(const std::string& model_name, const std::string& mesh_name);

        std::shared_ptr<Model> GetModel(const std::string& name);

        // TODO:
        // Entity storage + add entity ?
        // Add RM and Scene to App
        // Scene - prepare to render

        // TODO:
        // Mesh
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
