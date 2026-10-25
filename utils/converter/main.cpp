// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#define CLI_CONFIGURATION \
    CLI_REQUIRED_PROPERTY(input, SYMBOL(i), input_file, "Input file path", \
        std::string, BaseValidator) \
    CLI_REQUIRED_PROPERTY(output, SYMBOL(o), output_file, "Output file path", \
        std::string, BaseValidator) \
    CLI_REQUIRED_PROPERTY(convertion, SYMBOL(c), convertion, "Conversion type", \
        std::string, ListValidator, "to_json", "to_bin" ) \
    CLI_OPTIONAL_PROPERTY(format, SYMBOL(f), format, "Input file format", \
        std::string, "autodetect", ListValidator, \
            "autodetect", "animation", "scene" ) \
    CLI_OPTIONAL_PROPERTY(animation-position-eps, WO_SYMBOL, animation_position_eps, \
        "This parameter applies only when the input file format is an animation.\n" \
    "    Specifies the maximum allowable error for animation position channel compression.", \
        float, 0.0005f, BaseValidator) \
    CLI_OPTIONAL_PROPERTY(animation-rotation-eps, WO_SYMBOL, animation_rotation_eps, \
        "This parameter applies only when the input file format is an animation.\n" \
    "    Specifies the maximum allowable error for animation rotation channel compression.", \
        float, 0.0005f, BaseValidator) \
    CLI_OPTIONAL_PROPERTY(animation-quternion-size, WO_SYMBOL, animation_quternion_size, \
        "This parameter applies only when the input file format is an animation.\n" \
    "    Specifies the number of bits used to pack quaternions in the animation rotation channel.", \
        int, 32, ListValidator, 32, 64, 128)

#define CLI_ABOUT \
    "Copyright (c) 2026, Yakov Usoltsev\n" \
    "Email: yakovmen62@gmail.com\n" \
    "License: MIT"

#include <cu/cli-utils.hpp>

#include <qce/ancillary/extensions.hpp>

#include <qce/ecs/ecs.hpp>
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
        CU_ENUM_UNIT(scene) \
        CU_ENUM_UNIT(camera_system_config) \
        CU_ENUM_UNIT(hid_system_config) \
        CU_ENUM_UNIT(movement_system_config) \
        CU_ENUM_UNIT(render_system_config) \
        CU_ENUM_UNIT(graphics_output_config) \
        CU_ENUM_UNIT(common_config) \
        CU_END_ENUM(Format)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

static inline bool is_json(const std::filesystem::path& input_path) {
    return input_path.extension().string() == ".json";
}

static inline Format detect_format(const std::filesystem::path& input_path) {
    auto extension = input_path.extension().string();
    auto file_name = input_path.stem().string();
    if (is_json(input_path)) {
        extension = input_path.stem().extension().string();
        file_name = file_name.substr(0, file_name.find_last_of('.'));
    }

    if (extension.size() != 5 || !extension.starts_with('.')) {
        std::cerr << "Failed to detect format for file: " << input_path.string() << std::endl;
        return Format::E_Format_UNKNOWN;
    }
    extension.erase(0, 1); // remove dot

    if (QCE::ANIMATION_CONTAINER == extension) {
        std::cout << "Transform animation format detected" << std::endl;
        return Format::animation;
    }
    if (QCE::SCENE_CONTAINER == extension) {
        std::cout << "Scene description format detected" << std::endl;
        return Format::scene;
    }
    if (QCE::SYSTEM_CONFIG_CONTAINER == extension) {
        std::cout << "System config format detected" << std::endl;
        if ("camera_system" == file_name) {
            std::cout << "Camera system config format detected" << std::endl;
            return Format::camera_system_config;
        }
        if ("hid_system" == file_name) {
            std::cout << "HID system config format detected" << std::endl;
            return Format::hid_system_config;
        }
        if ("movement_system" == file_name) {
            std::cout << "Movement system config format detected" << std::endl;
            return Format::movement_system_config;
        }
        if ("render_system" == file_name) {
            std::cout << "Render system config format detected" << std::endl;
            return Format::render_system_config;
        }
        if ("graphics_output_config" == file_name) {
            std::cout << "Graphics output system config format detected" << std::endl;
            return Format::graphics_output_config;
        }
        if ("common_config" == file_name) {
            std::cout << "Common system config format detected" << std::endl;
            return Format::common_config;
        }

        std::cerr << "Unknown system config file name: " << file_name << std::endl;
        return Format::E_Format_UNKNOWN;
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

template<typename T, typename CtxT = QCE::Private::NoContext>
int convert_file(
        const std::filesystem::path& input_path,
        const std::filesystem::path& output_path,
        Convertion convertion,
        CtxT&& ctx = {}) {
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
        if (QCE::write_to_binary(output_path, data, std::forward<CtxT>(ctx)) != QCE::ErrorCode::SUCCESS) {
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
    case Format::animation: {
        QCE::TransformAnimationCompressionParams ctx{
            .position_eps = cli_config.animation_position_eps,
            .scale_eps = cli_config.animation_position_eps,
            .rotation_quantization =
                (cli_config.animation_quternion_size == 32) ?
                QCE::E_32_BIT_QQ :
                    (cli_config.animation_quternion_size == 64) ?
                    QCE::E_64_BIT_QQ : QCE::E_NO_QQ
        };
        return convert_file<QCE::TransformAnimation>(
            cli_config.input_file, cli_config.output_file, convertion, std::move(ctx));
    }
    case Format::scene:
        return convert_file<QCE::SceneDescription>(
            cli_config.input_file, cli_config.output_file, convertion);
    case Format::camera_system_config:
        return convert_file<QCE::CameraConfig>(
            cli_config.input_file, cli_config.output_file, convertion);
    case Format::hid_system_config:
        return convert_file<QCE::HidConfig>(
            cli_config.input_file, cli_config.output_file, convertion);
    case Format::movement_system_config:
        return convert_file<QCE::MovementConfig>(
            cli_config.input_file, cli_config.output_file, convertion);
    case Format::render_system_config:
        return convert_file<QCE::RenderConfig>(
            cli_config.input_file, cli_config.output_file, convertion);
    case Format::graphics_output_config:
        return convert_file<QCE::GraphicsOutputConfig>(
            cli_config.input_file, cli_config.output_file, convertion);
    case Format::common_config:
        return convert_file<QCE::CommonConfig>(
            cli_config.input_file, cli_config.output_file, convertion);
    default:
        assert(!"Unsupported format");
        return -1;
    }

    return 0;
}
