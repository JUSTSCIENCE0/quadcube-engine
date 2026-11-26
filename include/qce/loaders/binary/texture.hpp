// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/error_codes.hpp>
#include <qce/objects/texture.hpp>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM_TYPED(ImageContainer, int8_t) \
        CU_ENUM_UNIT(E_IMAGE_CONTAINER_PNG) \
        CU_ENUM_UNIT(E_IMAGE_CONTAINER_JPEG) \
    CU_END_ENUM(ImageContainer)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    ErrorCode import_texture(const uint8_t* data, size_t data_size, ImageContainer& container, Texture2D& texture);
}