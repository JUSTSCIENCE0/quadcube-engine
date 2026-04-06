// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// common
MJSON_ENUM_BEGIN(RenderType, "Render Type", nullptr)
    MJSON_ENUM_UNIT(E_RENDER_DIRECTX12, directx12)
    MJSON_ENUM_UNIT(E_RENDER_VULKAN, vulkan)
    MJSON_ENUM_VALUE(E_RENDER_UNKNOWN, unknown, -1)
MJSON_ENUM_END(RenderType)

// camera config
// TODO

// hid config
// TODO

// movement config
MJSON_OBJECT_BEGIN(
        MovementConfig,
        "Movement Config",
        "Configuration of the movement system")
    MJSON_FIELD(std::optional<std::string>, temporal, nullptr, nullptr)
MJSON_OBJECT_END(MovementConfig)

// render config
// TODO

#ifndef QCE_CONFIGS_ANCILLARY_H
#define QCE_CONFIGS_ANCILLARY_H

#  ifdef WIN32
    static constexpr auto DEFAULT_RENDER_TYPE = RenderType::E_RENDER_DIRECTX12;
#  else
    static constexpr auto DEFAULT_RENDER_TYPE = RenderType::E_RENDER_VULKAN;
#  endif

#endif // QCE_CONFIGS_ANCILLARY_H
