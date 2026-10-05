// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <cu/file-utils.hpp>

#include <qce/ancillary/error_codes.hpp>

namespace QCE {
    static inline std::filesystem::path get_directory(const std::string& directory_name) {
        std::filesystem::path result = CU::get_current_module_directory();
        result.append(directory_name);

        if (!std::filesystem::exists(result) ||
            !std::filesystem::is_directory(result)) {
            std::error_code fs_error;
            if (!std::filesystem::create_directory(result, fs_error)) {
                throw ErrorCodeException(E_ENG_BAD_CONFIGS_DIRECTORY);
            }
        }

        return result;
    }

    static inline std::filesystem::path get_configs_directory() {
        return get_directory("configs");
    }

    static inline std::filesystem::path get_scenes_directory() {
        return get_directory("scenes");
    }
}