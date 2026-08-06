// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/objects/resource_manager.hpp>
#include <qce/loaders/mjson/generated_objects.hpp>

namespace QCE {
    std::filesystem::path ResourceManager::GetResourcesDirectory() {
        std::filesystem::path result = CU::get_current_module_directory();
        result.append(RESOURCES_DIRECTORY);

        if (!std::filesystem::exists(result) ||
            !std::filesystem::is_directory(result))
            throw ErrorCodeException(E_RM_BAD_RESOURCES_DIRECTORY);

        return result;
    }

    void ResourceManager::RegisterDefaultCommands() {
        Add(Command{
            "Exit", std::make_shared<ExitCommand>()
        });
    }

    void ResourceManager::AddDefaultTextures() {
        QCE::Texture2D no_texture2d{};
        no_texture2d.id = "no_texture2d";
        QCE_SOFT(QCE::texture2d_color(WHITE, no_texture2d));
        Add(std::move(no_texture2d));

        QCE::Texture2D missed_texture2d{};
        missed_texture2d.id = "missed_texture2d";
        QCE_SOFT(QCE::texture2d_color(RED, missed_texture2d));
        Add(std::move(missed_texture2d));
    }

    ErrorCode ResourceManager::AddFigure(const FigureParams& params, const std::string& mesh_name) {
        if (!mesh_name.empty()) {
            if (Exists<Mesh>(mesh_name))
                return ErrorCode::E_RM_RESOURCE_ALREADY_EXISTS;
        }

        auto mesh = generate_figure(params, mesh_name);
        assert(
            !mesh.id.empty() &&
            !mesh.indices.empty() &&
            !mesh.vertices.empty());

        return Add(std::move(mesh));
    }

    ErrorCode ResourceManager::AddShader(
            const std::string& shader_name,
            ShaderType shader_type) {
        Shader shader{};
        QCE_CRITICAL(
            load_shader(
                shader_name,
                shader_type,
                m_shaders_bytecode_directory,
                m_render_type,
                shader
            )
        );

        return Add(std::move(shader));
    }

    ErrorCode ResourceManager::AddTexture(
            const std::string& texture_name
            /*TODO: TextureType texture_type = TextureType::TEXTURE_2D*/) {
        auto file_name = m_textures_directory / (texture_name + "." + TEXTURE_CONTAINER);
        QCE::Texture2D texture{};
        texture.id = texture_name;
        QCE_CRITICAL(QCE::texture2d_load(file_name, texture));

        return Add(std::move(texture));
    }

    ErrorCode ResourceManager::AddAnimation(
            const std::string& animation_name
            /*TODO: AnimationType animation_type = AnimationType::TRANSFORM_ANIMATION*/) {
        const auto file_name = m_animations_directory / (animation_name + "." + ANIMATION_CONTAINER);

        if (std::filesystem::exists(file_name)) {
            // TODO: try load from binary file
            // return ErrorCode::SUCCESS;
        }

        auto json_file_name = file_name;
        json_file_name += ".json";
        if (!std::filesystem::exists(json_file_name))
            return ErrorCode::E_RM_ANIMATION_NOT_FOUND;

        std::string error_descr;
        QCE::TransformAnimation animation{};
        if (macrojson::E_MJSON_OK != 
            macrojson::json_file_to_object(json_file_name, animation, error_descr)) {
            // TODO: use log system
            std::cout << "Failed to load animation from json file: " << json_file_name
                      << " with error: " << error_descr << std::endl;
            return ErrorCode::E_RM_ANIMATION_LOAD_FAILED;
        }

        QCE_CRITICAL(QCE::validate_animation(animation));
        return Add(std::move(animation));
    }
}
