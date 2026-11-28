// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#define CLI_CONFIGURATION \
    CLI_REQUIRED_PROPERTY(input, SYMBOL(i), input_file, "Input OBJ, glTF, or GLB file path", \
        std::string, BaseValidator) \
    CLI_REQUIRED_PROPERTY(conversion, SYMBOL(c), conversion, "Output format", \
        std::string, ListValidator, "to_json", "to_bin") \
    CLI_OPTIONAL_PROPERTY(mesh-output, WO_SYMBOL, mesh_output_directory, \
        "Output directory for meshes; meshes are not saved when omitted", \
        std::string, "", BaseValidator) \
    CLI_OPTIONAL_PROPERTY(material-output, WO_SYMBOL, material_output_directory, \
        "Output directory for binary materials; materials are not saved when omitted", \
        std::string, "", BaseValidator) \
    CLI_OPTIONAL_PROPERTY(texture-output, WO_SYMBOL, texture_output_directory, \
        "Output directory for KTX2 textures; textures are not saved when omitted", \
        std::string, "", BaseValidator)

#define CLI_ABOUT \
    "Copyright (c) 2026, Yakov Usoltsev\n" \
    "Email: yakovmen62@gmail.com\n" \
    "License: MIT"

#include <cu/cli-utils.hpp>

#include <qce/ancillary/extensions.hpp>
#include <qce/ancillary/error_codes.hpp>
#include <qce/loaders/binary/gltf.hpp>
#include <qce/loaders/binary/obj.hpp>
#include <qce/loaders/binary/texture.hpp>
#include <qce/loaders/write.hpp>

#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(Conversion) \
        CU_ENUM_UNIT(to_json) \
        CU_ENUM_UNIT(to_bin) \
    CU_END_ENUM(Conversion)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace {
    static inline bool ensure_output_directory_exists(const std::filesystem::path& output_directory) {
        std::error_code error;
        const auto output_path_exists = std::filesystem::exists(output_directory, error);
        if (error) {
            std::cerr << "Failed to inspect output path: " << output_directory.string()
                      << " " << error.message() << std::endl;
            return false;
        }
        if (output_path_exists) {
            const auto output_is_directory = std::filesystem::is_directory(output_directory, error);
            if (error) {
                std::cerr << "Failed to inspect output path: " << output_directory.string()
                          << " " << error.message() << std::endl;
                return false;
            }
            if (!output_is_directory) {
                std::cerr << "Output path is not a directory: "
                          << output_directory.string() << std::endl;
                return false;
            }
            return true;
        }

        if (std::filesystem::create_directories(output_directory, error))
            return true;

        if (error) {
            std::cerr << "Failed to create output directory: " << output_directory.string()
                      << " " << error.message() << std::endl;
            return false;
        }
        const auto output_is_directory = std::filesystem::is_directory(output_directory, error);
        if (error) {
            std::cerr << "Failed to inspect output path: " << output_directory.string()
                      << " " << error.message() << std::endl;
            return false;
        }
        return output_is_directory;
    }

    static inline bool is_single_filename_component(const std::string& name) {
        const std::filesystem::path path{name};
        return !name.empty() && name != "." && name != ".." &&
            !path.has_parent_path() && !path.has_root_name() &&
            !path.has_root_directory() && path.filename().string() == name;
    }

    static inline std::string make_collision_key(const std::filesystem::path& path) {
        auto key = path.filename().string();
        std::transform(key.begin(), key.end(), key.begin(),
            [](unsigned char character) { return static_cast<char>(std::tolower(character)); });
        return key;
    }

    static inline bool append_resource_output_path(
            const std::filesystem::path& output_directory,
            const std::string& mesh_id,
            const std::string& extension,
            std::set<std::string>& output_names,
            std::vector<std::filesystem::path>& output_paths) {
        if (!is_single_filename_component(mesh_id)) {
            std::cerr << "Resource name must be a single filename component: "
                      << mesh_id << std::endl;
            return false;
        }

        auto output_path = output_directory / (mesh_id + extension);
        if (!output_names.insert(make_collision_key(output_path)).second) {
            std::cerr << "Multiple resources map to the same output file: "
                      << output_path.string() << std::endl;
            return false;
        }

        std::error_code error;
        if (std::filesystem::exists(output_path, error)) {
            std::cerr << "Output file already exists: " << output_path.string() << std::endl;
            return false;
        }
        if (error) {
            std::cerr << "Failed to inspect output file: " << output_path.string()
                      << " " << error.message() << std::endl;
            return false;
        }

        output_paths.push_back(std::move(output_path));
        return true;
    }

    static inline bool prepare_mesh_output_paths(
            const std::vector<QCE::Mesh>& meshes,
            const std::filesystem::path& output_directory,
            const std::string& extension,
            std::vector<std::filesystem::path>& output_paths) {
        if (!ensure_output_directory_exists(output_directory)) {
            return false;
        }

        std::set<std::string> output_names;
        output_paths.clear();
        output_paths.reserve(meshes.size());
        for (const auto& mesh : meshes) {
            if (!append_resource_output_path(
                    output_directory, mesh.id, extension, output_names, output_paths)) {
                return false;
            }
        }
        return true;
    }

    static inline bool prepare_material_output_paths(
            const std::vector<QCE::Material>& materials,
            const std::filesystem::path& output_directory,
            std::vector<std::filesystem::path>& output_paths) {
        if (!ensure_output_directory_exists(output_directory)) {
            return false;
        }

        std::set<std::string> output_names;
        output_paths.clear();
        output_paths.reserve(materials.size());
        const auto extension = "." + std::string{QCE::MATERIAL_CONTAINER};
        for (const auto& material : materials) {
            if (!append_resource_output_path(
                    output_directory, material.id, extension, output_names, output_paths)) {
                return false;
            }
        }
        return true;
    }

    static inline bool prepare_texture_output_paths(
            const std::vector<QCE::Texture2D>& textures,
            const std::filesystem::path& output_directory,
            std::vector<std::filesystem::path>& output_paths) {
        if (!ensure_output_directory_exists(output_directory)) {
            return false;
        }

        std::set<std::string> output_names;
        output_paths.clear();
        output_paths.reserve(textures.size());
        const auto extension = "." + std::string{QCE::TEXTURE_CONTAINER};
        for (const auto& texture : textures) {
            if (!append_resource_output_path(
                    output_directory, texture.id, extension, output_names, output_paths)) {
                return false;
            }
        }
        return true;
    }

    static inline int import_model(
            const std::filesystem::path& input_path,
            const std::filesystem::path& mesh_output_directory,
            const std::filesystem::path& material_output_directory,
            const std::filesystem::path& texture_output_directory,
            Conversion conversion) {
        if (mesh_output_directory.empty() &&
            material_output_directory.empty() &&
            texture_output_directory.empty()) {
            std::cerr << "At least one output directory must be specified" << std::endl;
            return -1;
        }

        QCE::ModelResources resources{};
        std::vector<QCE::ModelDescription> model_descriptions{};
        auto input_extension = input_path.extension().string();
        std::transform(input_extension.begin(), input_extension.end(), input_extension.begin(),
            [](unsigned char character) { return static_cast<char>(std::tolower(character)); });
        if (!input_extension.empty() && input_extension.front() == '.')
            input_extension.erase(0, 1);

        QCE::ErrorCode load_result{};
        if (input_extension == QCE::OBJ_MODEL_CONTAINER)
            load_result = QCE::load_obj_file(input_path, resources, model_descriptions);
        else if (input_extension == QCE::GLTF_MODEL_CONTAINER ||
                 input_extension == QCE::GLB_MODEL_CONTAINER)
            load_result = QCE::load_gltf_file(input_path, resources, model_descriptions);
        else {
            std::cerr << "Unsupported input file extension: " << input_extension << std::endl;
            return -1;
        }

        if (load_result != QCE::ErrorCode::SUCCESS) {
            std::cerr << "Failed to load model file: " << input_path.string()
                      << " " << QCE::describe_error(load_result) << std::endl;
            return -1;
        }
        if (resources.meshes.empty()) {
            std::cerr << "Model file contains no meshes: " << input_path.string() << std::endl;
            return -1;
        }

        const auto extension = conversion == Conversion::to_json
            ? "." + std::string{QCE::MESH_CONTAINER} + ".json"
            : "." + std::string{QCE::MESH_CONTAINER};
        std::vector<std::filesystem::path> output_paths;
        if (!mesh_output_directory.empty() &&
            !prepare_mesh_output_paths(
                resources.meshes, mesh_output_directory, extension, output_paths)) {
            return -1;
        }

        std::vector<std::filesystem::path> material_output_paths;
        if (!material_output_directory.empty() &&
            !prepare_material_output_paths(
                resources.materials, material_output_directory, material_output_paths)) {
            return -1;
        }

        std::vector<std::filesystem::path> texture_output_paths;
        if (!texture_output_directory.empty() &&
            !prepare_texture_output_paths(
                resources.textures, texture_output_directory, texture_output_paths)) {
            return -1;
        }

        if (!mesh_output_directory.empty()) {
            for (std::size_t mesh_index = 0; mesh_index < resources.meshes.size(); ++mesh_index) {
                const auto& mesh = resources.meshes[mesh_index];
                const auto& output_path = output_paths[mesh_index];
                const auto write_result = conversion == Conversion::to_json
                    ? QCE::write_to_json(output_path, mesh)
                    : QCE::write_to_binary(output_path, mesh);
                if (write_result != QCE::ErrorCode::SUCCESS) {
                    std::cerr << "Failed to write mesh file: " << output_path.string()
                              << " " << QCE::describe_error(write_result) << std::endl;
                    return -1;
                }
                std::cout << "Wrote mesh: " << output_path.string() << std::endl;
            }
        }

        if (!material_output_directory.empty()) {
            for (std::size_t material_index = 0;
                    material_index < resources.materials.size(); ++material_index) {
                const auto& material = resources.materials[material_index];
                const auto& output_path = material_output_paths[material_index];
                const auto write_result = QCE::write_to_binary(output_path, material);
                if (write_result != QCE::ErrorCode::SUCCESS) {
                    std::cerr << "Failed to write material file: " << output_path.string()
                              << " " << QCE::describe_error(write_result) << std::endl;
                    return -1;
                }
                std::cout << "Wrote material: " << output_path.string() << std::endl;
            }
        }

        if (!texture_output_directory.empty()) {
            for (std::size_t texture_index = 0;
                    texture_index < resources.textures.size(); ++texture_index) {
                const auto& texture = resources.textures[texture_index];
                const auto& output_path = texture_output_paths[texture_index];
                const auto write_result = QCE::save_texture_to_ktx2(texture, output_path);
                if (write_result != QCE::ErrorCode::SUCCESS) {
                    std::cerr << "Failed to write texture file: " << output_path.string()
                              << " " << QCE::describe_error(write_result) << std::endl;
                    return -1;
                }
                std::cout << "Wrote texture: " << output_path.string() << std::endl;
            }
        }

        return 0;
    }
}

int main(int argc, char* argv[]) {
    CU::CLIConfig cli_config{};
    if (!CU::parse_cli_args(argc, argv, &cli_config))
        return -1;

    const auto conversion = Conversion_from_string(cli_config.conversion.c_str());
    if (conversion == Conversion::E_Conversion_UNKNOWN) {
        std::cerr << "Unknown conversion type: " << cli_config.conversion << std::endl;
        return 1;
    }

    return import_model(
        cli_config.input_file,
        cli_config.mesh_output_directory,
        cli_config.material_output_directory,
        cli_config.texture_output_directory,
        conversion);
}
