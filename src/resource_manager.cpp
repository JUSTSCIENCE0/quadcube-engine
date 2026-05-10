// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/objects/resource_manager.hpp>

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
}
