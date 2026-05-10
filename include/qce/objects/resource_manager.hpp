// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/error_codes.hpp>
#include <qce/objects/shader.hpp>
#include <qce/objects/texture.hpp>
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
#include <tuple>

namespace QCE {
    template<typename T>
    concept Resource = requires(T t) {
        requires std::same_as<
            std::remove_cvref_t<decltype(t.id)>,
            std::string
        >;
        requires !std::is_copy_constructible_v<T>;
        requires !std::is_copy_assignable_v<T>;
        requires std::is_move_constructible_v<T>;
        requires std::is_move_assignable_v<T>;
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

        ErrorCode AddFigure(const FigureParams& params, const std::string& mesh_name="");

        ErrorCode AddShader(
            const std::string& shader_name,
            ShaderType shader_type);

        ErrorCode AddTexture(
            const std::string& texture_name,
            const std::filesystem::path& file_name
            /*TODO: TextureType texture_type = TextureType::TEXTURE_2D*/
        );

        template <Resource ResourceT>
        ErrorCode Add(ResourceT resource) {
            auto& storage = std::get<ResourceStorage<ResourceT>>(m_storages);
            return storage.Add(std::move(resource));
        }

        template <Resource ResourceT>
        bool Exists(size_t index) {
            auto& storage = std::get<ResourceStorage<ResourceT>>(m_storages);
            return storage.Exists(index);
        }

        template <Resource ResourceT>
        bool Exists(const std::string& id) {
            auto& storage = std::get<ResourceStorage<ResourceT>>(m_storages);
            return storage.Exists(id);
        }

        template <Resource ResourceT>
        size_t GetIndex(const std::string& id) {
            auto& storage = std::get<ResourceStorage<ResourceT>>(m_storages);
            return storage.GetIndex(id);
        }

        template <Resource ResourceT>
        ResourceT& Read(size_t index) {
            auto& storage = std::get<ResourceStorage<ResourceT>>(m_storages);
            return storage.Get(index);
        }

        template <Resource ResourceT>
        ResourceT& Read(const std::string& id) {
            auto index = GetIndex<ResourceT>(id);
            return Read<ResourceT>(index);
        }

        template <Resource ResourceT>
        ErrorCode RemoveResource(size_t index) {
            auto& storage = std::get<ResourceStorage<ResourceT>>(m_storages);
            return storage.Remove(index);
        }

        template <Resource ResourceT>
        ErrorCode RemoveResource(const std::string& id) {
            auto& storage = std::get<ResourceStorage<ResourceT>>(m_storages);
            return storage.Remove(id);
        }

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

                return (m_free_indices.end() == m_free_indices.find(index));
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

        template<typename... Rs>
        using Resources = std::tuple<ResourceStorage<Rs>...>;

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

        Resources<
            Mesh,
            Shader,
            Texture2D,
            Command
        > m_storages{};
    };
}
