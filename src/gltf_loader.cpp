// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/loaders/binary/gltf.hpp>
#include <qce/loaders/binary/texture.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
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
    // Keep glTF indices at 32 bits until validation, before narrowing to QCE::index_t.
    using GltfIndex = std::uint32_t;

    static inline bool get_byte_view(
            const fastgltf::DataSource& source,
            const std::uint8_t*& data,
            std::size_t& size) {
        const std::byte* bytes = nullptr;
        if (const auto* array = std::get_if<fastgltf::sources::Array>(&source)) {
            bytes = array->bytes.data();
            size = array->bytes.size();
        } else if (const auto* vector = std::get_if<fastgltf::sources::Vector>(&source)) {
            bytes = vector->bytes.data();
            size = vector->bytes.size();
        } else if (const auto* byte_view = std::get_if<fastgltf::sources::ByteView>(&source)) {
            bytes = byte_view->bytes.data();
            size = byte_view->bytes.size();
        } else
            return false;

        data = reinterpret_cast<const std::uint8_t*>(bytes);
        return true;
    }

    static inline bool get_image_byte_view(
            const fastgltf::Asset& asset,
            const fastgltf::Image& image,
            const std::uint8_t*& data,
            std::size_t& size) {
        if (get_byte_view(image.data, data, size))
            return true;

        const auto* image_buffer_view =
            std::get_if<fastgltf::sources::BufferView>(&image.data);
        if (!image_buffer_view ||
            image_buffer_view->bufferViewIndex >= asset.bufferViews.size())
            return false;

        const auto& buffer_view = asset.bufferViews[image_buffer_view->bufferViewIndex];
        if (buffer_view.bufferIndex >= asset.buffers.size())
            return false;

        const std::uint8_t* buffer_data = nullptr;
        std::size_t buffer_size = 0;
        if (!get_byte_view(asset.buffers[buffer_view.bufferIndex].data, buffer_data, buffer_size) ||
            buffer_view.byteOffset > buffer_size ||
            buffer_view.byteLength > buffer_size - buffer_view.byteOffset)
            return false;

        data = buffer_data
            ? buffer_data + buffer_view.byteOffset
            : nullptr;
        size = buffer_view.byteLength;
        return true;
    }

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

    static inline bool append_triangle_indices(
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

    static inline ErrorCode load_primitive(
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

    static inline float3d convert_fresnel(
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

    static inline float convert_shininess(const fastgltf::Material& source) {
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

    static inline Material load_material(const fastgltf::Material& source, std::string id) {
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

    static inline ErrorCode load_albedo_texture(
            const fastgltf::Asset& asset,
            const fastgltf::Material& source_material,
            Material& material,
            const std::filesystem::path& path,
            std::set<std::string>& texture_ids,
            std::vector<std::optional<std::size_t>>& imported_texture_indices,
            std::vector<Texture2D>& textures) {
        if (!source_material.pbrData.baseColorTexture.has_value())
            return ErrorCode::SUCCESS;

        const auto texture_index = source_material.pbrData.baseColorTexture->textureIndex;
        if (texture_index >= asset.textures.size())
            REPORT_GLTF_ERROR(path, "Material references an invalid base color texture.");

        if (!imported_texture_indices[texture_index].has_value()) {
            const auto& source_texture = asset.textures[texture_index];
            if (!source_texture.imageIndex.has_value() ||
                source_texture.imageIndex.value() >= asset.images.size())
                REPORT_GLTF_ERROR(path, "Base color texture references an invalid image.");

            const auto image_index = source_texture.imageIndex.value();
            const auto& source_image = asset.images[image_index];
            const std::uint8_t* image_data = nullptr;
            std::size_t image_data_size = 0;
            if (!get_image_byte_view(asset, source_image, image_data, image_data_size))
                REPORT_GLTF_ERROR(path, "Base color image data is unavailable or invalid.");

            ImageContainer image_container{};
            Texture2D texture{};
            QCE_CRITICAL(import_texture(image_data, image_data_size, image_container, texture));

            const auto file_stem = path.stem().string();
            texture.id = make_unique_id(
                source_texture.name.empty() ? source_image.name : source_texture.name,
                file_stem + "_texture", texture_ids);
            imported_texture_indices[texture_index] = textures.size();
            textures.push_back(std::move(texture));
        }

        material.albedo_texture = imported_texture_indices[texture_index].value();
        return ErrorCode::SUCCESS;
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
            fastgltf::Options::LoadExternalImages |
            fastgltf::Options::GenerateMeshIndices;
        fastgltf::Parser parser(supported_extensions);
        auto asset_result = parser.loadGltf(gltf_data.get(), path.parent_path(), options);
        if (asset_result.error() != fastgltf::Error::None)
            REPORT_GLTF_ERROR(path, fastgltf::getErrorMessage(asset_result.error()));

        const auto& asset = asset_result.get();
        const auto file_stem = path.stem().string();
        resources.materials.reserve(asset.materials.size());
        std::set<std::string> material_ids;
        std::set<std::string> texture_ids;
        std::vector<std::optional<std::size_t>> imported_texture_indices(asset.textures.size());
        resources.textures.reserve(asset.textures.size());
        for (const auto& source_material : asset.materials) {
            auto material_id = make_unique_id(source_material.name, file_stem + "_material", material_ids);
            auto material = load_material(source_material, std::move(material_id));
            QCE_CRITICAL(
                load_albedo_texture(
                    asset,
                    source_material,
                    material,
                    path,
                    texture_ids,
                    imported_texture_indices,
                    resources.textures));

            // TODO: Load metallic-roughness, normal, occlusion, emissive, and extension textures.
            resources.materials.push_back(std::move(material));
        }

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
                model_descriptions.push_back({
                    loaded_mesh.id,
                    std::move(material_id)
                });
                resources.meshes.push_back(std::move(loaded_mesh));
            }
        }

        return ErrorCode::SUCCESS;
    }
}

#undef REPORT_GLTF_ERROR
