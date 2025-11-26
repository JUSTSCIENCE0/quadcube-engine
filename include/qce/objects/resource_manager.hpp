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
#include <memory>

namespace QCE {
    class ResourceManager final {
    public:
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;
        ResourceManager& operator=(ResourceManager&&) = delete;

        // singleton access
        static void Initialize(RenderType render_type) {
            assert(m_instance == nullptr);
            m_instance = std::unique_ptr<ResourceManager>(
                new ResourceManager(render_type));
        }
        static ResourceManager& Get() {
            assert(m_instance != nullptr);
            return *m_instance;
        }

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
            const Transform& start_transform = {});
        ErrorCode AddShader(
            const std::string& shader_name,
            ShaderType shader_type);

        std::shared_ptr<Entity> AddAndGetEntity(
            const std::string& entity_name,
            const std::string& model_name,
            const Transform& start_transform = {});

        std::shared_ptr<Model> GetModel(const std::string& name);
        std::shared_ptr<Shader> GetShader(const std::string& shader_name, ShaderType shader_type);

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
        /// ctor
        explicit ResourceManager(
            RenderType render_type) :
                m_render_type(render_type),
                m_resources_directory(GetResourcesDirectory()),
                m_shaders_bytecode_directory(m_resources_directory / SHADERS_BYTECODE_SUBDIRECTORY) {}

        /// singleton
        static inline std::unique_ptr<ResourceManager> m_instance = nullptr;

        /// types
        template <typename T>
        using Storage = std::unordered_map<std::string, std::shared_ptr<T>>;

        /// consts
        static constexpr auto RESOURCES_DIRECTORY = "resources";
        static constexpr auto SHADERS_BYTECODE_SUBDIRECTORY = "shaders";

        /// methods
        static std::filesystem::path GetResourcesDirectory();

        /// attributes
        RenderType m_render_type = DEFAULT_RENDER_TYPE;
        const std::filesystem::path m_resources_directory;
        const std::filesystem::path m_shaders_bytecode_directory;

        Storage<Mesh>   m_meshes{};
        Storage<Model>  m_models{};
        Storage<Entity> m_entities{};
        Storage<Shader> m_shaders{};
    };
}
