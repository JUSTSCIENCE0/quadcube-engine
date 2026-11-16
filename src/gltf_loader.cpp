// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/loaders/binary/gltf.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#define REPORT_GLTF_ERROR(path, message) \
    do { /*TODO: use log system*/ \
        std::cerr << "Failed to load glTF file: " << (path).string() \
                  << " - " << (message) << std::endl; \
        return ErrorCode::E_ENG_FILE_READ_FAILED; \
    } while (false)

namespace QCE {
    namespace {
        // Keep glTF indices at 32 bits until validation, before narrowing to QCE::index_t.
        using GltfIndex = std::uint32_t;

        static inline std::string make_unique_id(
                std::string id,
                const std::string& fallback,
                std::set<std::string>& used_ids) {
            if (id.empty())
                id = fallback;
            if (used_ids.insert(id).second)
                return id;

            for (std::size_t suffix = 1; ; ++suffix) {
                auto unique_id = id + "_" + std::to_string(suffix);
                if (used_ids.insert(unique_id).second)
                    return unique_id;
            }
        }

        bool append_triangle_indices(
                fastgltf::PrimitiveType type,
                const std::vector<GltfIndex>& source_indices,
                std::vector<index_t>& output_indices) {
            const auto append_triangle = [&](GltfIndex a, GltfIndex b, GltfIndex c) {
                output_indices.push_back(static_cast<index_t>(a));
                output_indices.push_back(static_cast<index_t>(b));
                output_indices.push_back(static_cast<index_t>(c));
            };

            switch (type) {
                case fastgltf::PrimitiveType::Triangles:
                    if (source_indices.size() % 3 != 0)
                        return false;
                    for (std::size_t index = 0; index < source_indices.size(); index += 3)
                        append_triangle(
                            source_indices[index],
                            source_indices[index + 1],
                            source_indices[index + 2]);
                    return true;

                case fastgltf::PrimitiveType::TriangleStrip:
                    for (std::size_t index = 0; index + 2 < source_indices.size(); ++index) {
                        if (index % 2 == 0) {
                            append_triangle(
                                source_indices[index],
                                source_indices[index + 1],
                                source_indices[index + 2]);
                        } else {
                            append_triangle(
                                source_indices[index + 1],
                                source_indices[index],
                                source_indices[index + 2]);
                        }
                    }
                    return true;

                case fastgltf::PrimitiveType::TriangleFan:
                    for (std::size_t index = 1; index + 1 < source_indices.size(); ++index)
                        append_triangle(
                            source_indices[0],
                            source_indices[index],
                            source_indices[index + 1]);
                    return true;

                default:
                    return false;
            }
        }

        ErrorCode load_primitive(
                const fastgltf::Asset& asset,
                const fastgltf::Primitive& primitive,
                const std::filesystem::path& path,
                Mesh& mesh) {
            const auto position_attribute = primitive.findAttribute("POSITION");
            if (position_attribute == primitive.attributes.end())
                REPORT_GLTF_ERROR(path, "Primitive has no POSITION attribute.");

            const auto& position_accessor = asset.accessors[position_attribute->accessorIndex];
            if (fastgltf::validateAccessorBounds(asset, position_accessor) != fastgltf::Error::None)
                REPORT_GLTF_ERROR(path, "POSITION accessor references invalid buffer data.");

            constexpr auto max_index = static_cast<std::uint64_t>(
                std::numeric_limits<index_t>::max());
            constexpr auto max_vertex_count = max_index + 1;
            if (position_accessor.count > max_vertex_count)
                REPORT_GLTF_ERROR(path, "Primitive exceeds the configured mesh index width.");

            mesh.vertices.resize(position_accessor.count);
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                asset, position_accessor, [&](fastgltf::math::fvec3 position, std::size_t index) {
                    mesh.vertices[index].position.x() = position.x();
                    mesh.vertices[index].position.y() = position.y();
                    mesh.vertices[index].position.z() = position.z();
                });

            const auto normal_attribute = primitive.findAttribute("NORMAL");
            if (normal_attribute != primitive.attributes.end()) {
                const auto& normal_accessor = asset.accessors[normal_attribute->accessorIndex];
                if (normal_accessor.count != mesh.vertices.size() ||
                    fastgltf::validateAccessorBounds(asset, normal_accessor) != fastgltf::Error::None)
                    REPORT_GLTF_ERROR(path, "NORMAL accessor is invalid or has a different vertex count.");

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                    asset, normal_accessor, [&](fastgltf::math::fvec3 normal, std::size_t index) {
                        mesh.vertices[index].normal.x() = normal.x();
                        mesh.vertices[index].normal.y() = normal.y();
                        mesh.vertices[index].normal.z() = normal.z();
                    });
            }

            const auto texcoord_attribute = primitive.findAttribute("TEXCOORD_0");
            if (texcoord_attribute != primitive.attributes.end()) {
                const auto& texcoord_accessor = asset.accessors[texcoord_attribute->accessorIndex];
                if (texcoord_accessor.count != mesh.vertices.size() ||
                    fastgltf::validateAccessorBounds(asset, texcoord_accessor) != fastgltf::Error::None)
                    REPORT_GLTF_ERROR(
                        path, "TEXCOORD_0 accessor is invalid or has a different vertex count.");

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
                    asset, texcoord_accessor, [&](fastgltf::math::fvec2 texcoord, std::size_t index) {
                        mesh.vertices[index].texture_coordinates.x() = texcoord.x();
                        mesh.vertices[index].texture_coordinates.y() = texcoord.y();
                    });
            }

            std::vector<GltfIndex> source_indices;
            if (primitive.indicesAccessor.has_value()) {
                const auto& index_accessor = asset.accessors[*primitive.indicesAccessor];
                if (fastgltf::validateAccessorBounds(asset, index_accessor) != fastgltf::Error::None)
                    REPORT_GLTF_ERROR(path, "Index accessor references invalid buffer data.");

                source_indices.resize(index_accessor.count);
                fastgltf::copyFromAccessor<GltfIndex>(
                    asset, index_accessor, source_indices.data());
            } else {
                // Without an indices accessor, glTF consumes vertices sequentially (0..vertexCount-1).
                if (mesh.vertices.size() >
                    static_cast<std::uint64_t>(std::numeric_limits<GltfIndex>::max()) + 1) {
                    REPORT_GLTF_ERROR(path, "Primitive has too many vertices for 32-bit indices.");
                }
                source_indices.reserve(mesh.vertices.size());
                for (std::size_t index = 0; index < mesh.vertices.size(); ++index)
                    source_indices.push_back(static_cast<GltfIndex>(index));
            }

            for (const auto index : source_indices) {
                if (index >= mesh.vertices.size() || index > max_index)
                    REPORT_GLTF_ERROR(
                        path, "Primitive index is outside the vertex range or configured index width.");
            }

            if (!append_triangle_indices(primitive.type, source_indices, mesh.indices))
                REPORT_GLTF_ERROR(
                    path, "Only triangle, triangle-strip, and triangle-fan primitives are supported.");

            return ErrorCode::SUCCESS;
        }

        float3d convert_fresnel(
                const fastgltf::Material& source,
                const color_rgba& base_color) {
            const auto ior = static_cast<float>(source.ior);
            const auto ior_ratio = (ior - 1.0f) / (ior + 1.0f);
            const auto ior_f0 = ior_ratio * ior_ratio;
            const auto metallic = std::clamp(
                static_cast<float>(source.pbrData.metallicFactor), 0.0f, 1.0f);
            const auto specular_factor = source.specular.has_value()
                ? static_cast<float>(source.specular->specularFactor)
                : 1.0f;
            const auto specular_color_factor = [&](std::size_t channel) {
                return source.specular.has_value()
                    ? static_cast<float>(source.specular->specularColorFactor[channel])
                    : 1.0f;
            };
            const auto dielectric_f0 = [&](std::size_t channel) {
                return std::min(
                    ior_f0 * specular_color_factor(channel), 1.0f) * specular_factor;
            };

            float3d fresnel{};
            fresnel.x() = dielectric_f0(0) * (1.0f - metallic) + base_color.r() * metallic;
            fresnel.y() = dielectric_f0(1) * (1.0f - metallic) + base_color.g() * metallic;
            fresnel.z() = dielectric_f0(2) * (1.0f - metallic) + base_color.b() * metallic;
            return fresnel;
        }

        float convert_shininess(const fastgltf::Material& source) {
            const auto roughness = std::clamp(
                static_cast<float>(source.pbrData.roughnessFactor), 0.0f, 1.0f);
            if (roughness == 0.0f)
                return 1.0f;

            constexpr float max_exponent = 256.0f;
            const auto roughness_squared = roughness * roughness;
            const auto exponent = std::clamp(
                2.0f / (roughness_squared * roughness_squared) - 2.0f,
                0.0f,
                max_exponent);
            return exponent / max_exponent;
        }

        Material load_material(const fastgltf::Material& source, std::string id) {
            Material material{};
            material.id = std::move(id);
            material.albedo_color.r() =
                static_cast<float>(source.pbrData.baseColorFactor[0]);
            material.albedo_color.g() =
                static_cast<float>(source.pbrData.baseColorFactor[1]);
            material.albedo_color.b() =
                static_cast<float>(source.pbrData.baseColorFactor[2]);
            material.albedo_color.a() =
                static_cast<float>(source.pbrData.baseColorFactor[3]);
            material.fresnel = convert_fresnel(source, material.albedo_color);
            material.shininess = convert_shininess(source);
            return material;
        }
    }

    ErrorCode load_gltf_file(
            const std::filesystem::path& path,
            ModelResources& resources,
            std::vector<ModelDescription>& model_descriptions) {
        resources = {};
        model_descriptions.clear();

        if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
            return ErrorCode::E_ENG_FILE_NOT_FOUND;

        auto gltf_data = fastgltf::GltfDataBuffer::FromPath(path);
        if (gltf_data.error() != fastgltf::Error::None)
            REPORT_GLTF_ERROR(path, fastgltf::getErrorMessage(gltf_data.error()));

        constexpr auto supported_extensions =
            fastgltf::Extensions::KHR_mesh_quantization |
            fastgltf::Extensions::KHR_materials_ior |
            fastgltf::Extensions::KHR_materials_specular;
        constexpr auto options =
            fastgltf::Options::LoadExternalBuffers |
            fastgltf::Options::GenerateMeshIndices;
        fastgltf::Parser parser(supported_extensions);
        auto asset_result = parser.loadGltf(gltf_data.get(), path.parent_path(), options);
        if (asset_result.error() != fastgltf::Error::None)
            REPORT_GLTF_ERROR(path, fastgltf::getErrorMessage(asset_result.error()));

        const auto& asset = asset_result.get();
        const auto file_stem = path.stem().string();
        resources.materials.reserve(asset.materials.size());
        std::set<std::string> material_ids;
        for (const auto& source_material : asset.materials) {
            auto material_id = make_unique_id(source_material.name, file_stem + "_material", material_ids);
            resources.materials.push_back(load_material(source_material, std::move(material_id)));
        }

        bool has_albedo_texture = false;
        resources.meshes.reserve(asset.meshes.size());
        std::set<std::string> mesh_ids;
        for (const auto& source_mesh : asset.meshes) {
            for (const auto& primitive : source_mesh.primitives) {
                auto mesh_id = make_unique_id(source_mesh.name, file_stem + "_mesh", mesh_ids);

                Mesh loaded_mesh{};
                loaded_mesh.id = std::move(mesh_id);
                const auto load_result = load_primitive(asset, primitive, path, loaded_mesh);
                if (load_result != ErrorCode::SUCCESS)
                    return load_result;

                std::string material_id;
                if (primitive.materialIndex.has_value()) {
                    if (primitive.materialIndex.value() >= resources.materials.size())
                        REPORT_GLTF_ERROR(path, "Primitive references an invalid material.");
                    material_id = resources.materials[primitive.materialIndex.value()].id;
                }
                if (primitive.materialIndex.has_value()) {
                    const auto& material = asset.materials[primitive.materialIndex.value()];
                    has_albedo_texture = has_albedo_texture ||
                        material.pbrData.baseColorTexture.has_value();
                    // TODO: Check metallic-roughness, normal, occlusion, emissive, and extension textures.
                }

                model_descriptions.push_back({
                    loaded_mesh.id,
                    std::move(material_id)
                });
                resources.meshes.push_back(std::move(loaded_mesh));
            }
        }

        if (has_albedo_texture) {
            std::cerr << "glTF albedo texture loading is not implemented yet; "
                         "geometry and materials were loaded without texture data: "
                      << path.string() << '\n';
        }

        return ErrorCode::SUCCESS;
    }
}

#undef REPORT_GLTF_ERROR
