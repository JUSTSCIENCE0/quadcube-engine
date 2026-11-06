// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/loaders/binary/obj.hpp>

#include <tiny_obj_loader.h>

#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <tuple>
#include <utility>

namespace QCE {
    namespace {
        inline bool is_valid_obj_index(int index, std::size_t count) {
            return index >= 0 && static_cast<std::size_t>(index) < count;
        }

        inline bool is_valid_optional_obj_index(int index, std::size_t count) {
            return index == -1 || is_valid_obj_index(index, count);
        }
    }

    // TODO: add support for materials and textures
    ErrorCode load_obj_file(const std::filesystem::path& path, std::vector<Mesh>& meshes) {
        if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
            return ErrorCode::E_ENG_FILE_NOT_FOUND;
        }

        tinyobj::ObjReaderConfig config{};
        config.triangulate = true;

        tinyobj::ObjReader reader{};
        if (!reader.ParseFromFile(path.string(), config)) {
            // TODO: use log system
            std::cerr << "Failed to load OBJ file: " << path.string() << '\n'
                      << reader.Error() << '\n';
            return ErrorCode::E_ENG_FILE_READ_FAILED;
        }
        if (!reader.Warning().empty()) {
            // TODO: use log system
            std::cerr << "OBJ loader warning: " << reader.Warning() << '\n';
        }

        const auto& attrib = reader.GetAttrib();
        using ObjIndex = std::tuple<int, int, int>;
        const auto max_index = static_cast<std::size_t>(std::numeric_limits<index_t>::max());
        std::vector<Mesh> loaded_meshes;
        const auto& shapes = reader.GetShapes();
        loaded_meshes.reserve(shapes.size());

        for (const auto& shape: shapes) {
            Mesh loaded_mesh{};
            loaded_mesh.id = shape.name;
            std::map<ObjIndex, std::size_t> vertex_map;

            for (const auto& obj_index : shape.mesh.indices) {
                if (!is_valid_obj_index(obj_index.vertex_index, attrib.vertices.size() / 3)) {
                    // TODO: use log system
                    std::cerr << "OBJ file contains an invalid position index: " << path.string() << '\n';
                    return ErrorCode::E_ENG_FILE_READ_FAILED;
                }
                if (!is_valid_optional_obj_index(obj_index.normal_index, attrib.normals.size() / 3)) {
                    // TODO: use log system
                    std::cerr << "OBJ file contains an invalid normal index: " << path.string() << '\n';
                    return ErrorCode::E_ENG_FILE_READ_FAILED;
                }
                if (!is_valid_optional_obj_index(obj_index.texcoord_index, attrib.texcoords.size() / 2)) {
                    // TODO: use log system
                    std::cerr << "OBJ file contains an invalid texture coordinate index: " << path.string() << '\n';
                    return ErrorCode::E_ENG_FILE_READ_FAILED;
                }

                const ObjIndex key{
                    obj_index.vertex_index,
                    obj_index.normal_index,
                    obj_index.texcoord_index
                };
                const auto existing = vertex_map.find(key);
                if (existing != vertex_map.end()) {
                    loaded_mesh.indices.push_back(static_cast<index_t>(existing->second));
                    continue;
                }

                if (loaded_mesh.vertices.size() > max_index) {
                    // TODO: use log system
                    std::cerr << "OBJ file exceeds the configured mesh index width: "
                              << path.string() << '\n';
                    return ErrorCode::E_ENG_FILE_READ_FAILED;
                }

                const auto vertex_index = loaded_mesh.vertices.size();
                vertex loaded_vertex{};

                const auto position_offset = obj_index.vertex_index * 3;
                for (index_t component = 0; component < 3; ++component) {
                    loaded_vertex.position.arr[component] =
                        attrib.vertices[position_offset + component];
                }

                if (obj_index.normal_index >= 0) {
                    const auto normal_offset = obj_index.normal_index * 3;
                    for (index_t component = 0; component < 3; ++component) {
                        loaded_vertex.normal.arr[component] =
                            attrib.normals[normal_offset + component];
                    }
                }

                if (obj_index.texcoord_index >= 0) {
                    const auto texcoord_offset = obj_index.texcoord_index * 2;
                    loaded_vertex.texture_coordinates.x() = attrib.texcoords[texcoord_offset];
                    loaded_vertex.texture_coordinates.y() = attrib.texcoords[texcoord_offset + 1];
                }

                loaded_mesh.vertices.push_back(loaded_vertex);
                vertex_map.emplace(key, vertex_index);
                loaded_mesh.indices.push_back(static_cast<index_t>(vertex_index));
            }

            loaded_meshes.push_back(std::move(loaded_mesh));
        }

        meshes = std::move(loaded_meshes);
        return ErrorCode::SUCCESS;
    }
}
