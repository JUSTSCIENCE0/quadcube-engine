// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/renders/render_type.hpp>
#include <qce/ancillary/error_codes.hpp>
#include <cu/file-utils.hpp>

#include <stdint.h>
#include <string>
#include <filesystem>
#include <vector>
#include <cassert>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(ShaderType) \
        CU_ENUM_UNIT(E_VERTEX_SHADER) \
        CU_ENUM_UNIT(E_PIXEL_SHADER) \
        CU_ENUM_ANCILLARY_UNITS(E_SHADERS_TYPE) \
    CU_END_ENUM(ShaderType)
    /*
    TODO:
        E_TESSELLATION_CONTROL_SHADER
        E_TESSELLATION_EVALUATION_SHADER
        E_GEOMETRY_SHADER
        E_MESH_SHADER
    */
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    struct Shader {
        std::string id{};
        ShaderType type = ShaderType::E_ShaderType_UNKNOWN;
        std::filesystem::path bytecode_file{};
        std::vector<uint8_t> bytecode_cache{};
    };

    static inline const char* get_shader_language(RenderType render_type) {
        switch (render_type) {
        case RenderType::E_RENDER_DIRECTX12:
            return "hlsl";
        case RenderType::E_RENDER_VULKAN:
            return "glsl";
        default:
            assert(!"Unexpected behavior");
            break;
        }

        return "";
    }

    static inline const char* get_shader_bytecode_type(RenderType render_type) {
        switch (render_type) {
        case RenderType::E_RENDER_DIRECTX12:
            return "cso";
        case RenderType::E_RENDER_VULKAN:
            return "spv";
        default:
            assert(!"Unexpected behavior");
            break;
        }

        return "";
    }

    static inline const char* get_shader_type_suffix(ShaderType shader_type) {
        switch (shader_type) {
        case ShaderType::E_VERTEX_SHADER:
            return "vs";
        case ShaderType::E_PIXEL_SHADER:
            return "ps";
        default:
            assert(!"Unexpected behavior");
            break;
        }

        return "";
    }

    static inline std::string make_shader_id(
            const std::string& name,
            ShaderType shader_type) {
        return name + "_" + get_shader_type_suffix(shader_type);
    }

    static std::filesystem::path make_shader_bytecode_filepath(
        const std::filesystem::path& dir,
        const std::string& id,
        RenderType render_type) {
        std::filesystem::path result = dir;
        const char* lang = get_shader_language(render_type);
        std::string filename = id + "." + get_shader_bytecode_type(render_type);
        result.append(lang);
        result.append(filename);

        return result;
    }

    static inline ErrorCode load_shader(
            const std::string& id,
            ShaderType shader_type,
            const std::filesystem::path& bytecode_dir,
            RenderType render_type,
            Shader& result) {
        result.id = make_shader_id(id, shader_type);
        result.type = shader_type;
        result.bytecode_file = make_shader_bytecode_filepath(
            bytecode_dir, result.id, render_type);
        result.bytecode_cache = CU::load_data_from_file<uint8_t>(result.bytecode_file);

        if (result.bytecode_cache.empty()) {
            return ErrorCode::E_ENG_SHADER_BYTECODE_NOT_FOUND;
        }

        return ErrorCode::SUCCESS;
    }
}
