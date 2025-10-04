// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/renders/render_type.hpp>

#include <stdint.h>
#include <string>
#include <filesystem>
#include <vector>
#include <cassert>

namespace QCE {
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

    struct Shader {
        Shader(
                std::string name,
                std::string entry,
                const std::filesystem::path& source_dir,
                const std::filesystem::path& binary_dir,
                RenderType render_type) :
            m_id(std::move(name)),
            m_entry_point(std::move(entry)),
            m_source_file(make_shader_filepath(source_dir, m_id, render_type)),
            m_binary_file(make_shader_filepath(binary_dir, m_id, render_type)) {
            // TODO: load binary cache if exists else load source cache
        }

        const std::string m_id;
        const std::string m_entry_point;

        const std::filesystem::path m_source_file;
        const std::filesystem::path m_binary_file;

        std::vector<uint8_t> m_source_cache{};
        std::vector<uint8_t> m_binary_cache{};
        // TODO: m_binary_metadata

    private:
        static std::filesystem::path make_shader_filepath(
                const std::filesystem::path& dir,
                const std::string& name,
                RenderType render_type) {
            std::filesystem::path result = dir;
            const char* lang = get_shader_language(render_type);
            std::string filename = name + "." + lang;
            result.append(lang);
            result.append(filename);

            return result;
        }
    };
}
