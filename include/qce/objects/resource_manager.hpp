// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/error_codes.hpp>
#include <qce/objects/entity.hpp>
#include <qce/objects/figures.hpp>
#include <qce/objects/command.hpp>

#include <cu/file-utils.hpp>

#include <unordered_map>
#include <concepts>
#include <type_traits>
#include <memory>
#include <vector>
#include <set>
#include <limits>

namespace QCE {
    template<typename T>
    concept Resource = requires(T t) {
        requires std::same_as<
            std::remove_cvref_t<decltype(t.id)>,
            std::string
        >;
    };

    class ResourceManager final {
    public:
        static constexpr auto INVALID_RESOURCE_INDEX = std::numeric_limits<size_t>::max();

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
        ErrorCode AddCommand(std::unique_ptr<Command>&& command);

        std::shared_ptr<Entity> AddAndGetEntity(
            const std::string& entity_name,
            const std::string& model_name,
            const Transform& start_transform = {});

        std::shared_ptr<Model> GetModel(const std::string& name);
        std::shared_ptr<Shader> GetShader(const std::string& shader_name, ShaderType shader_type);
        std::shared_ptr<Command> GetCommand(const std::string& command_name);

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
        explicit ResourceManager(RenderType render_type) :
            m_render_type(render_type),
            m_resources_directory(GetResourcesDirectory()),
            m_shaders_bytecode_directory(m_resources_directory / SHADERS_BYTECODE_SUBDIRECTORY) {
            RegisterDefaultCommands();
        }

        /// singleton
        static inline std::unique_ptr<ResourceManager> m_instance = nullptr;

        /// types
        template <typename T>
        using Storage = std::unordered_map<std::string, std::shared_ptr<T>>;

        template <Resource ResourceT>
        class ResourceStorage {
        public:
            ErrorCode Add(ResourceT resource) {
                std::string id = resource.id;

                if (m_index.end() != m_index.find(id))
                    return ErrorCode::E_RM_RESOURCE_ALREADY_EXISTS;

                if (!m_free_indices.empty()) {
                    auto index = *m_free_indices.begin();
                    m_data[index] = std::move(resource);
                    m_index.emplace(std::move(id), index);
                    m_free_indices.erase(index);
                }
                else {
                    m_data.push_back(std::move(resource));
                    m_index.emplace(std::move(id), m_data.size() - 1);
                }

                return ErrorCode::SUCCESS;
            }

            bool Exists(size_t index) {
                if (m_data.size() <= index)
                    return false;

                return (m_free_indices.end() != m_free_indices.find(index));
            }

            bool Exists(const std::string& id) {
                return (m_index.end() != m_index.find(id));
            }

            size_t GetIndex(const std::string& id) {
                auto result = m_index.find(id);
                if (m_index.end() == result)
                    return INVALID_RESOURCE_INDEX;
                return result->second;
            }

            ResourceT& Get(size_t index) {
                assert(Exists(index));
                return m_data[index];
            }

            ErrorCode Remove(size_t index) {
                if (!Exists(index))
                    return ErrorCode::E_RM_RESOURCE_NOT_FOUND;

                std::string id = m_data[index].id;
                m_index.erase(id);

                m_free_indices.insert(index);

                if constexpr (requires(ResourceT rs) { rs.reset(); }) {
                    m_data[index].reset();
                }

                return ErrorCode::SUCCESS;
            }

            ErrorCode Remove(const std::string& id) {
                size_t index = GetIndex(id);
                if (INVALID_RESOURCE_INDEX == index)
                    return ErrorCode::E_RM_RESOURCE_NOT_FOUND;

                Remove(index);
            }

        private:
            std::vector<ResourceT> m_data{};
            std::unordered_map<std::string, size_t> m_index{};
            std::set<size_t> m_free_indices{};
        };

        /// consts
        static constexpr auto RESOURCES_DIRECTORY = "resources";
        static constexpr auto SHADERS_BYTECODE_SUBDIRECTORY = "shaders";

        /// methods
        static std::filesystem::path GetResourcesDirectory();
        void RegisterDefaultCommands();

        /// attributes
        RenderType m_render_type = DEFAULT_RENDER_TYPE;
        const std::filesystem::path m_resources_directory;
        const std::filesystem::path m_shaders_bytecode_directory;

        Storage<Mesh>    m_meshes{};
        Storage<Model>   m_models{};
        Storage<Entity>  m_entities{};
        Storage<Shader>  m_shaders{};
        Storage<Command> m_commands{};
    };
}
