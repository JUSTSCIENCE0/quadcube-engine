// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/error_codes.hpp>

#include <qce/loaders/binary/resources.hpp>
#include <qce/loaders/mjson/generated_objects.hpp>

#include <fstream>

namespace QCE {
    // defaults
    template<typename T>
    ErrorCode write_to_binary(const std::filesystem::path& path, T&& object) {
        std::fstream s{ path, std::ios::out | std::ios::binary | std::ios::trunc };
        if (!s.is_open())
            return ErrorCode::E_ENG_FILE_OPEN_FAILED;

        try {
            bitsery::Serializer<bitsery::OutputBufferedStreamAdapter> ser{ s };
            ser.object(object);
            ser.adapter().flush();
            s.close();
        }
        catch (std::exception& exp) {
            std::cout << "Failed to write to binary file: " << path.string()
                << " with error: " << exp.what() << std::endl;
            return ErrorCode::E_ENG_FILE_WRITE_FAILED;
        }

        if (!s.good()) {
            std::cout << "Failed to write to binary file: " << path.string()
                << " with unknown error" << std::endl;
            return ErrorCode::E_ENG_FILE_WRITE_FAILED;
        }

        return ErrorCode::SUCCESS;
    }

    template<typename T>
    ErrorCode write_to_json(const std::filesystem::path& path, T&& object) {
        auto code = macrojson::object_to_json_file(object, path);
        if (code != macrojson::E_MJSON_OK) {
            return ErrorCode::E_ENG_FILE_WRITE_FAILED;
        }

        return ErrorCode::SUCCESS;
    }
}