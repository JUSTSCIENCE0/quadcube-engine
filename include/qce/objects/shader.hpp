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

namespace QCE {
    struct Shader {
        const std::string m_id;
        const std::string m_entry_point;

        std::filesystem::path m_source_file{};
        std::filesystem::path m_binary_file{};

        std::vector<uint8_t> m_binary_cache{};


    };
}
