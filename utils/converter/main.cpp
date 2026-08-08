// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#define CLI_CONFIGURATION \
    CLI_REQUIRED_PROPERTY(input, SYMBOL(i), input_file, "input file path", \
        std::string, BaseValidator) \
    CLI_REQUIRED_PROPERTY(output, SYMBOL(o), output_file, "output file path", \
        std::string, BaseValidator) \
    CLI_REQUIRED_PROPERTY(convertion, SYMBOL(c), convertion, "convertion type", \
        std::string, ListValidator, "to_json", "to_bin" ) \
    CLI_OPTIONAL_PROPERTY(format, SYMBOL(f), format, "input file format", \
        std::string, "autodetect", ListValidator, "autodetect", "animation" )

#define CLI_ABOUT \
    "Copyright (c) 2026, Yakov Usoltsev\n" \
    "Email: yakovmen62@gmail.com\n" \
    "License: MIT"

#include <cu/cli-utils.hpp>

#include <qce/objects/resource_manager.hpp>

#include <qce/loaders/read.hpp>
#include <qce/loaders/write.hpp>

#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(Convertion) \
        CU_ENUM_UNIT(to_json) \
        CU_ENUM_UNIT(to_bin) \
    CU_END_ENUM(Convertion) \
    CU_BEGIN_ENUM(Format) \
        CU_ENUM_UNIT(autodetect) \
        CU_ENUM_UNIT(animation) \
    CU_END_ENUM(Format)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

static inline bool is_json(const std::filesystem::path& input_path) {
    return input_path.extension().string() == ".json";
}

static inline Format detect_format(const std::filesystem::path& input_path) {
    auto extension = input_path.extension().string();
    if (is_json(input_path)) {
        extension = input_path.stem().extension().string();
    }

    if (extension.size() != 5 || !extension.starts_with('.')) {
        std::cerr << "Failed to detect format for file: " << input_path.string() << std::endl;
        return Format::E_Format_UNKNOWN;
    }
    extension.erase(0, 1); // remove dot

    if (QCE::ResourceManager::ANIMATION_CONTAINER == extension) {
        std::cout << "Transform animation format detected" << std::endl;
        return Format::animation;
    }

    std::cerr << "Unknown file extension: " << extension << std::endl;
    return Format::E_Format_UNKNOWN;
}

template<typename T>
bool load_file(const std::filesystem::path& input_path, T& data) {
    if (is_json(input_path)) {
        return QCE::read_from_json(input_path, data) == QCE::ErrorCode::SUCCESS;
    }

    return QCE::read_from_binary(input_path, data) == QCE::ErrorCode::SUCCESS;
}

template<typename T>
int convert_file(
        const std::filesystem::path& input_path,
        const std::filesystem::path& output_path,
        Convertion convertion) {
    T data{};
    if (!load_file(input_path, data)) {
        std::cerr << "Failed to load file: " << input_path.string() << std::endl;
        return -1;
    }

    switch (convertion) {
    case Convertion::to_json:
        if (QCE::write_to_json(output_path, data) != QCE::ErrorCode::SUCCESS) {
            std::cerr << "Failed to write JSON file: " << output_path.string() << std::endl;
            return -1;
        }
        break;
    case Convertion::to_bin:
        if (QCE::write_to_binary(output_path, data) != QCE::ErrorCode::SUCCESS) {
            std::cerr << "Failed to write binary file: " << output_path.string() << std::endl;
            return -1;
        }
        break;
    default:
        std::cerr << "Unknown convertion type" << std::endl;
        return -1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    CU::CLIConfig cli_config{};
    if (!CU::parse_cli_args(argc, argv, &cli_config))
        return -1;

    auto format = Format_from_string(cli_config.format.c_str());
    const auto convertion = Convertion_from_string(cli_config.convertion.c_str());

    if (Format::autodetect == format) {
        format = detect_format(cli_config.input_file);
        if (Format::E_Format_UNKNOWN == format)
            return -1;
    }

    switch (format) {
    case Format::animation:
        return convert_file<QCE::TransformAnimation>(
            cli_config.input_file, cli_config.output_file, convertion);
    default:
        assert(!"Unsupported format");
        return -1;
    }

    return 0;
}
