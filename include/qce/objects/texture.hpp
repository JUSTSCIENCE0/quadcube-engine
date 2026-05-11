// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/error_codes.hpp>

#include <stdint.h>

#include <filesystem>
#include <optional>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(TextureFormat) \
        CU_VALUED_ENUM_UNIT(E_TEXFMT_UNKNOWN, -1) \
        CU_VALUED_ENUM_UNIT(E_TEXFMT_BC7_UNORM_BLOCK, 145) \
        CU_VALUED_ENUM_UNIT(E_TEXFMT_BC7_SRGB_BLOCK, 146) \
    CU_END_ENUM(TextureFormat)
// TODO: enum TextureType
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    struct Texture2D {
        Texture2D() = default;
        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;
        Texture2D(Texture2D&& other) noexcept;
        Texture2D& operator=(Texture2D&& other) noexcept;
        ~Texture2D();

        std::string id{};

        TextureFormat format = TextureFormat::E_TEXFMT_UNKNOWN;
        uint32_t base_width  = 0;
        uint32_t base_height = 0;

        struct MipLevel {
            uint32_t width  = 0;
            uint32_t height = 0;
            uint32_t row_pitch = 0;
            uint32_t slice_pitch = 0;
            void* data = nullptr;
        };
        std::vector<MipLevel> mip_levels;

        std::optional<size_t> gpu_texture_index = std::nullopt;
        void* ktx = nullptr;
    };

    ErrorCode texture2d_load(const std::filesystem::path& path, Texture2D& texture);
}
