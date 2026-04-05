// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/renders/render_type.hpp>
#include <qce/objects/shader.hpp>

#include <string>
#include <vector>

namespace QCE {
    struct RenderConfig {
        RenderType render_type = DEFAULT_RENDER_TYPE;

        int width = 1280;
        int height = 720;

        struct ShaderDescr {
            std::string shader_name{};
            ShaderType  shader_type = ShaderType::E_ShaderType_UNKNOWN;
        };
        std::vector<ShaderDescr> used_shaders{};

        // ancillary
        void* window = nullptr;
        void* app = nullptr;
    };
}
