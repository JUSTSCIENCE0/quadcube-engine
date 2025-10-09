// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/error_codes.hpp>
#include <qce/objects/entity.hpp>
#include <qce/objects/figures.hpp>

#include <cu/file-utils.hpp>

#include <unordered_map>
#include <concepts>
#include <type_traits>

namespace QCE {
    class ResourceManager final {
    public:
        ResourceManager(
                RenderType render_type) :
            m_render_type(render_type),
            m_resources_directory(GetResourcesDirectory()),
            m_shaders_source_directory(m_resources_directory / SHADERS_SOURCE_SUBDIRECTORY),
            m_shaders_binary_directory(m_resources_directory / SHADERS_BINARY_SUBDIRECTORY) {}

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
        ErrorCode AddModel(
            const std::string& model_name,
            const std::string& mesh_name);
        ErrorCode AddEntity(
            const std::string& entity_name,
            const std::string& model_name,
            const transform& start_transform = {});
        ErrorCode AddShader(
            const std::string& shader_name,
            const std::string& entry_point);

        std::shared_ptr<Entity> AddAndGetEntity(
            const std::string& entity_name,
            const std::string& model_name,
            const transform& start_transform = {});

        std::shared_ptr<Model> GetModel(const std::string& name);

        // TODO:
        // Mesh
        //  error mesh
        //  GetMesh
        //  Load Mesh (from file)
        // Model
        //  error model
        //  Load Model (from file)
        //  Add Model (as shared_ptr)
        // Entity
        //  Add Entity (as shared_ptr)

    private:
        /// types
        template <typename T>
        using Storage = std::unordered_map<std::string, std::shared_ptr<T>>;

        /// consts
        static constexpr auto RESOURCES_DIRECTORY = "resources";
        static constexpr auto SHADERS_SOURCE_SUBDIRECTORY = "shaders/source";
        static constexpr auto SHADERS_BINARY_SUBDIRECTORY = "shaders/binary";

        /// methods
        static std::filesystem::path GetResourcesDirectory();

        /// attributes
        RenderType m_render_type = DEFAULT_RENDER_TYPE;
        const std::filesystem::path m_resources_directory{ RESOURCES_DIRECTORY };
        const std::filesystem::path m_shaders_source_directory;
        const std::filesystem::path m_shaders_binary_directory;

        Storage<Mesh>   m_meshes{};
        Storage<Model>  m_models{};
        Storage<Entity> m_entities{};
        Storage<Shader> m_shaders{};
    };
}
