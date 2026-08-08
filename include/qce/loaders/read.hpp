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
    ErrorCode read_from_binary(const std::filesystem::path& path, T& object) {
        if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
            return ErrorCode::E_ENG_FILE_NOT_FOUND;

        std::fstream s{ path, std::ios::in | std::ios::binary };
        if (!s.is_open())
            return ErrorCode::E_ENG_FILE_OPEN_FAILED;

        auto state = bitsery::quickDeserialization<bitsery::InputStreamAdapter>(s, object);

        if (state.first != bitsery::ReaderError::NoError || !state.second) {
            // TODO: use log system
            std::cout << "Failed to load from binary file: " << path.string()
                << " with error: " << int(state.first) << std::endl;
            return ErrorCode::E_ENG_FILE_READ_FAILED;
        }

        return ErrorCode::SUCCESS;
    }

    template<typename T>
    ErrorCode read_from_json(const std::filesystem::path& path, T& object) {
        if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
            return ErrorCode::E_ENG_FILE_NOT_FOUND;

        std::string error_descr;
        if (macrojson::E_MJSON_OK !=
            macrojson::json_file_to_object(path, object, error_descr)) {
            // TODO: use log system
            std::cout << "Failed to load from json file: " << path.string()
                << " with error: " << error_descr << std::endl;
            return ErrorCode::E_ENG_FILE_READ_FAILED;
        }

        return ErrorCode::SUCCESS;
    }

    template<typename T>
    ErrorCode read_from_file(const std::filesystem::path& path, T& object) {
        auto code = read_from_binary(path, object);
        if (code == ErrorCode::SUCCESS || code != ErrorCode::E_ENG_FILE_NOT_FOUND)
            return code;

        auto json_file_name = path;
        json_file_name += ".json";
        return read_from_json(json_file_name, object);
    }
}