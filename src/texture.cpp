// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/objects/texture.hpp>

#include <ktx.h>

#include <iostream>

namespace QCE {
    ErrorCode texture_load(const std::filesystem::path& path /*, Texture& texture*/) {
        if (!std::filesystem::exists(path)) {
            return ErrorCode::E_RM_TEXTURE_NOT_FOUND;
        }

        ktxTexture* ktx_texture = nullptr;
        auto ktx_result = ktxTexture_CreateFromNamedFile(
            path.string().c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktx_texture);
        if (KTX_SUCCESS != ktx_result) {
            // TODO: use log system
            std::cerr << "KTX: Failed to load texture: " << ktxErrorString(ktx_result) << '\n';
            return ErrorCode::E_RM_TEXTURE_LOAD_FAILED;
        }

        std::cout << "Levels: "  << ktx_texture->numLevels << "\n";
        std::cout << "Layers: "  << ktx_texture->numLayers << "\n";
        std::cout << "Faces: "   << ktx_texture->numFaces  << "\n";
        std::cout << "IsArray: " << ktx_texture->isArray   << "\n";

        ktxTexture_Destroy(ktx_texture);
        return ErrorCode::SUCCESS;
    }
}