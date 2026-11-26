// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/loaders/binary/texture.hpp>
#include <stb_image.h>

#include <compressonator.h>
#include <ktx.h>

#include <cstring>
#include <iostream>
#include <limits>
#include <mutex>
#include <utility>
#include <vector>

namespace QCE {
    constexpr int MIP_MIN_DIMENSION = 16;

    struct ScopedMipSet {
        CMP_MipSet value{};

        ~ScopedMipSet() {
            CMP_FreeMipSet(&value);
        }
    };

    struct ScopedStbiImage {
        stbi_uc* value = nullptr;

        ~ScopedStbiImage() {
            stbi_image_free(value);
        }
    };

    struct ScopedKtxTexture {
        ktxTexture2* value = nullptr;

        ~ScopedKtxTexture() {
            if (value) ktxTexture2_Destroy(value);
        }
    };

    static inline bool is_supported_image(const uint8_t* data, size_t data_size, ImageContainer& container) {
        constexpr uint8_t png_signature[] = {
            0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A
        };
        if (data_size >= sizeof(png_signature) &&
            std::memcmp(data, png_signature, sizeof(png_signature)) == 0) {
            container = ImageContainer::E_IMAGE_CONTAINER_PNG;
            return true;
        }
        if (data_size >= 3 && data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF) {
            container = ImageContainer::E_IMAGE_CONTAINER_JPEG;
            return true;
        }
        return false;
    }

    static inline void initialize_compressonator() {
        static std::once_flag initialization_flag;
        std::call_once(initialization_flag, [] { CMP_InitFramework(); });
    }

    ErrorCode import_texture(const uint8_t* data, size_t data_size, ImageContainer& container, Texture2D& texture) {
        if (!data || data_size == 0 || data_size > static_cast<size_t>(std::numeric_limits<int>::max()))
            return ErrorCode::E_ENG_BAD_ARG;

        const auto stbi_data_size = static_cast<int>(data_size);
        ImageContainer detected_container;
        if (!is_supported_image(data, data_size, detected_container))
            return ErrorCode::E_ENG_UNSUPPORTED_CONTAINER;

        int width = 0;
        int height = 0;
        int source_channels = 0;
        if (!stbi_info_from_memory(data, stbi_data_size, &width, &height, &source_channels))
            return ErrorCode::E_ENG_INVALID_IMAGE_DATA;
        if (width <= 0 || height <= 0)
            return ErrorCode::E_ENG_INVALID_IMAGE_DATA;

        const auto rgba_data_size =
            static_cast<std::uint64_t>(width) * static_cast<std::uint64_t>(height) * 4;
        if (rgba_data_size > static_cast<std::uint64_t>(std::numeric_limits<CMP_INT>::max()))
            return ErrorCode::E_ENG_INVALID_IMAGE_DATA;

        ScopedStbiImage rgba_image;
        int decoded_width = 0;
        int decoded_height = 0;
        rgba_image.value = stbi_load_from_memory(
            data, stbi_data_size, &decoded_width, &decoded_height, &source_channels, STBI_rgb_alpha);
        if (!rgba_image.value || decoded_width != width || decoded_height != height)
            return ErrorCode::E_RM_TEXTURE_PARSING_FAILED;

        initialize_compressonator();

        ScopedMipSet source;
        auto cmp_status = CMP_CreateMipSet(&source.value, width, height, 1, CF_8bit, TT_2D);
        if (cmp_status != CMP_OK) {
            // TODO: use log system
            std::cerr << "Compressonator: Failed to create source mip set (error "
                      << cmp_status << ").\n";
            return ErrorCode::E_RM_TEXTURE_CREATE_FAILED;
        }

        source.value.m_format = CMP_FORMAT_RGBA_8888;
        source.value.m_TextureDataType = TDT_ARGB;
        source.value.m_nChannels = 4;

        CMP_MipLevel* source_mip = nullptr;
        CMP_GetMipLevel(&source_mip, &source.value, 0, 0);
        if (!source_mip || !source_mip->m_pbData ||
            source_mip->m_dwLinearSize < rgba_data_size) {
            // TODO: use log system
            std::cerr << "Compressonator: Source mip level has insufficient storage.\n";
            return ErrorCode::E_RM_TEXTURE_CREATE_FAILED;
        }
        std::memcpy(source_mip->m_pbData, rgba_image.value, static_cast<size_t>(rgba_data_size));
        source.value.m_nMipLevels = 1;

        cmp_status = static_cast<CMP_ERROR>(
            CMP_GenerateMIPLevels(&source.value, MIP_MIN_DIMENSION));
        if (cmp_status != CMP_OK) {
            // TODO: use log system
            std::cerr << "Compressonator: Failed to generate mip levels (error "
                      << cmp_status << ").\n";
            return ErrorCode::E_RM_TEXTURE_CREATE_FAILED;
        }

        CMP_CompressOptions options{};
        options.dwSize = sizeof(options);
        options.SourceFormat = source.value.m_format;
        options.DestFormat = CMP_FORMAT_BC7;
        options.fquality = 0.8f;
        options.dwnumThreads = 0;

        ScopedMipSet compressed;
        cmp_status = CMP_ConvertMipTexture(&source.value, &compressed.value, &options, nullptr);
        if (cmp_status != CMP_OK) {
            // TODO: use log system
            std::cerr << "Compressonator: Failed to compress texture to BC7 (error "
                      << cmp_status << ").\n";
            return ErrorCode::E_RM_TEXTURE_CREATE_FAILED;
        }
        if (compressed.value.m_nMipLevels <= 0) {
            // TODO: use log system
            std::cerr << "Compressonator: Compression produced no mip levels.\n";
            return ErrorCode::E_RM_TEXTURE_CREATE_FAILED;
        }

        ktxTextureCreateInfo create_info{};
        create_info.vkFormat = static_cast<ktx_uint32_t>(TextureFormat::E_TEXFMT_BC7_UNORM_BLOCK);
        create_info.baseWidth = static_cast<ktx_uint32_t>(compressed.value.m_nWidth);
        create_info.baseHeight = static_cast<ktx_uint32_t>(compressed.value.m_nHeight);
        create_info.baseDepth = 1;
        create_info.numDimensions = 2;
        create_info.numLevels = static_cast<ktx_uint32_t>(compressed.value.m_nMipLevels);
        create_info.numLayers = 1;
        create_info.numFaces = 1;
        create_info.isArray = KTX_FALSE;
        create_info.generateMipmaps = KTX_FALSE;

        ScopedKtxTexture ktx_texture;
        auto ktx_status = ktxTexture2_Create(
            &create_info,
            KTX_TEXTURE_CREATE_ALLOC_STORAGE,
            &ktx_texture.value);
        if (ktx_status != KTX_SUCCESS) {
            // TODO: use log system
            std::cerr << "KTX2: Failed to create texture: " << ktxErrorString(ktx_status) << ".\n";
            return ErrorCode::E_RM_TEXTURE_CREATE_FAILED;
        }

        Texture2D imported_texture;
        imported_texture.format = TextureFormat::E_TEXFMT_BC7_UNORM_BLOCK;
        imported_texture.base_width = static_cast<uint32_t>(compressed.value.m_nWidth);
        imported_texture.base_height = static_cast<uint32_t>(compressed.value.m_nHeight);
        imported_texture.mip_levels.reserve(static_cast<size_t>(compressed.value.m_nMipLevels));

        for (CMP_INT level = 0; level < compressed.value.m_nMipLevels; ++level) {
            CMP_MipLevel* compressed_mip = nullptr;
            CMP_GetMipLevel(&compressed_mip, &compressed.value, level, 0);
            if (!compressed_mip || !compressed_mip->m_pbData) {
                // TODO: use log system
                std::cerr << "Compressonator: Compressed mip level " << level << " is missing.\n";
                return ErrorCode::E_RM_TEXTURE_CREATE_FAILED;
            }

            ktx_status = ktxTexture_SetImageFromMemory(
                ktxTexture(ktx_texture.value), static_cast<ktx_uint32_t>(level), 0, 0, compressed_mip->m_pbData,
                static_cast<ktx_size_t>(compressed_mip->m_dwLinearSize));
            if (ktx_status != KTX_SUCCESS) {
                // TODO: use log system
                std::cerr << "KTX2: Failed to set mip level " << level << ": "
                          << ktxErrorString(ktx_status) << ".\n";
                return ErrorCode::E_RM_TEXTURE_CREATE_FAILED;
            }

            ktx_size_t offset = 0;
            ktx_status = ktxTexture_GetImageOffset(
                ktxTexture(ktx_texture.value), static_cast<ktx_uint32_t>(level), 0, 0, &offset);
            if (ktx_status != KTX_SUCCESS) {
                // TODO: use log system
                std::cerr << "KTX2: Failed to get mip level " << level << " offset: "
                          << ktxErrorString(ktx_status) << ".\n";
                return ErrorCode::E_RM_TEXTURE_CREATE_FAILED;
            }

            auto& imported_mip = imported_texture.mip_levels.emplace_back();
            imported_mip.width = static_cast<uint32_t>(compressed_mip->m_nWidth);
            imported_mip.height = static_cast<uint32_t>(compressed_mip->m_nHeight);
            imported_mip.row_pitch = ((imported_mip.width + 3) / 4) * 16;
            imported_mip.slice_pitch = imported_mip.row_pitch * ((imported_mip.height + 3) / 4);
            imported_mip.data = ktx_texture.value->pData + offset;
        }

        imported_texture.ktx = ktx_texture.value;
        ktx_texture.value = nullptr;
        texture = std::move(imported_texture);
        container = detected_container;

        return ErrorCode::SUCCESS;
    }
}
