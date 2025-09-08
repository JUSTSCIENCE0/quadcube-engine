// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/model.hpp>
#include <qce/objects/figures.hpp>

#include <unordered_map>
#include <concepts>
#include <type_traits>

namespace QCE {
    class ResourseManager final {
    public:
        ResourseManager(const ResourseManager&) = delete;
        ResourseManager(ResourseManager&&) = delete;
        ResourseManager& operator=(const ResourseManager&) = delete;
        ResourseManager& operator=(ResourseManager&&) = delete;

        template<typename SharedMesh>
        requires std::same_as<std::remove_cvref_t<SharedMesh>, std::shared_ptr<Mesh>>
        void AddMesh(SharedMesh&& mesh) {
            if (m_meshes.end() != m_meshes.find(mesh->m_id))
                return;

            m_meshes.emplace(mesh->m_id, std::forward<SharedMesh>(mesh));
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
