// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/objects/texture.hpp>

#include <ktx.h>

#include <iostream>

namespace QCE {
    Texture2D::Texture2D(Texture2D&& other) noexcept :
        id(std::move(other.id)),
        format(other.format),
        base_width(other.base_width),
        base_height(other.base_height),
        mip_levels(std::move(other.mip_levels)),
        gpu_texture_index(std::move(other.gpu_texture_index)),
        ktx(other.ktx) {
        other.ktx = nullptr;
    }

    Texture2D& Texture2D::operator=(Texture2D&& other) noexcept {
        if (this != &other) {
            id = std::move(other.id);
            format = other.format;
            base_width = other.base_width;
            base_height = other.base_height;
            mip_levels = std::move(other.mip_levels);
            gpu_texture_index = std::move(other.gpu_texture_index);
            ktx = other.ktx;
            other.ktx = nullptr;
        }
        return *this;
    }

    Texture2D::~Texture2D() {
        if (ktx) {
            ktxTexture2_Destroy(static_cast<ktxTexture2 *>(ktx));
            ktx = nullptr;
        }
    }

    ErrorCode texture2d_load(const std::filesystem::path& path, Texture2D& texture) {
        if (!std::filesystem::exists(path)) {
            return ErrorCode::E_RM_TEXTURE_NOT_FOUND;
        }

        ktxTexture2* ktx_texture = nullptr;
        auto ktx_result = ktxTexture2_CreateFromNamedFile(
            path.string().c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktx_texture);
        if (KTX_SUCCESS != ktx_result) {
            // TODO: use log system
            std::cerr << "KTX: Failed to load texture: " << ktxErrorString(ktx_result) << '\n';
            return ErrorCode::E_RM_TEXTURE_LOAD_FAILED;
        }

        // TODO: check if format is supported
        texture.format = static_cast<TextureFormat>(ktx_texture->vkFormat);

        texture.base_width  = ktx_texture->baseWidth;
        texture.base_height = ktx_texture->baseHeight;

        texture.mip_levels.reserve(ktx_texture->numLevels);
        for (uint32_t level = 0; level < ktx_texture->numLevels; level++) {
            auto& mip_level = texture.mip_levels.emplace_back();
            mip_level.width  = std::max(1u, ktx_texture->baseWidth  >> level);
            mip_level.height = std::max(1u, ktx_texture->baseHeight >> level);
            mip_level.row_pitch = ((mip_level.width + 3) / 4) * 16 ;
            mip_level.slice_pitch = mip_level.row_pitch * ((mip_level.height + 3) / 4);

            ktx_size_t offset;
            ktx_result = ktxTexture_GetImageOffset(ktxTexture(ktx_texture), level, 0, 0, &offset);
             if (KTX_SUCCESS != ktx_result) {
                // TODO: use log system
                std::cerr << "KTX: Failed to get MIP Level #" << level
                          << " offset. Error: " << ktxErrorString(ktx_result) << '\n';

                ktxTexture2_Destroy(ktx_texture);
                return ErrorCode::E_RM_TEXTURE_PARSING_FAILED;
            }
            mip_level.data = ktx_texture->pData + offset;
        }

        texture.ktx = ktx_texture;
        return ErrorCode::SUCCESS;
    }
}