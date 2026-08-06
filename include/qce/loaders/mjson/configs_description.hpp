// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// common
MJSON_ENUM_BEGIN(RenderType,
        "Render Type", "Rendering system API")
    MJSON_ENUM_UNIT(E_RENDER_DIRECTX12, directx12)
    MJSON_ENUM_UNIT(E_RENDER_VULKAN, vulkan)
    MJSON_ENUM_VALUE(E_RENDER_UNKNOWN, unknown, -1)
MJSON_ENUM_END(RenderType)

// camera config
MJSON_ENUM_BEGIN(CameraType, "Camera Type", nullptr)
    MJSON_ENUM_UNIT(E_CAMERA_FIXED, fixed)
    MJSON_ENUM_UNIT(E_CAMERA_FIRST_PERSON, first_person)
    MJSON_ENUM_UNIT(E_CAMERA_THIRD_PERSON, third_person)
MJSON_ENUM_END(CameraType)

MJSON_OBJECT_BEGIN(
        CameraConfigUnit,
        "Camera Config Unit",
        "Configuration of a single camera")
    MJSON_FIELD(float, aspect,  "Aspect Ratio", nullptr)
    MJSON_FIELD(float, fov_rad, "FOV", "FOV in radians")
    MJSON_FIELD(float, znear,   "Near Clip", nullptr)
    MJSON_FIELD(float, zfar,    "Far Clip", nullptr)

    MJSON_FIELD(std::vector<float>, position, "Position", nullptr)
    MJSON_FIELD(std::vector<float>, target,   "Target", nullptr)
    MJSON_FIELD(std::vector<float>, up,       "Up", "Up direction")

    MJSON_FIELD(std::string, camera_name, "Camera name", nullptr)
    MJSON_FIELD(CameraType,  camera_type, "Camera Type", nullptr)
MJSON_OBJECT_END(CameraConfigUnit)

MJSON_OBJECT_BEGIN(
        CameraConfig,
        "Camera Config",
        "Configuration of the camera system")
    MJSON_FIELD(std::vector<CameraConfigUnit>, cameras,
                "Cameras", "Description of all cameras")
MJSON_OBJECT_END(CameraConfig)

// hid config
MJSON_ENUM_BEGIN(HidEventType, "Hid Event Type", nullptr)
    MJSON_ENUM_UNIT (E_HET_SINGLE,  single)
    MJSON_ENUM_UNIT (E_HET_ACCORD,  accord)
    MJSON_ENUM_UNIT (E_HET_COMBO,   combo)
    MJSON_ENUM_VALUE(E_HET_UNKNOWN, unknown, -1)
MJSON_ENUM_END(HidEventType)

#define MJSON_BASE_OBJECT_NAME HidEventDescriptor
MJSON_POLYMORPHIC_OBJECT_BEGIN(
        "HID Event Descriptor",
        "HID Event Descriptor Base Class")
    MJSON_BASE_OBJECT_BEGIN(HidEventType)
      MJSON_BASE_OBJECT_FIELD(std::string, handler,
        "Handler",
        "Handler name for this event")
    MJSON_BASE_OBJECT_END()
    MJSON_DERIVED_OBJECT_BEGIN(HidSingleEvent, E_HET_SINGLE)
      MJSON_DERIVED_OBJECT_FIELD(std::string, hid_event_code_name,
        "HID Event Code Name",
        "Name of the HID event code\n"
        "Structure: <INPUT_DEVICE>_<EVENT>")
    MJSON_DERIVED_OBJECT_END(HidSingleEvent)
    // TODO - HidAccordEvent, HidComboEvent
MJSON_POLYMORPHIC_OBJECT_END()
#undef MJSON_BASE_OBJECT_NAME

MJSON_OBJECT_BEGIN(
        HidConfig,
        "HID Config",
        "Configuration of the HID system")
    MJSON_FIELD(
        std::vector<std::unique_ptr<HidEventDescriptor>>,
        event_descriptors, nullptr, nullptr)
MJSON_OBJECT_END(HidConfig)

// movement config
MJSON_OBJECT_BEGIN(
        MovementConfig,
        "Movement Config",
        "Configuration of the movement system")
    MJSON_FIELD(std::optional<std::string>, temporal, nullptr, nullptr)
MJSON_OBJECT_END(MovementConfig)

// render config
MJSON_ENUM_BEGIN(ShaderType, "Shader Type", nullptr)
    MJSON_ENUM_UNIT (E_VERTEX_SHADER, vertex)
    MJSON_ENUM_UNIT (E_PIXEL_SHADER,  pixel)
// ancillary
    MJSON_ENUM_UNIT (E_SHADERS_TYPE_COUNT, count)
    MJSON_ENUM_VALUE(E_UNKNOWN_SHADER, unknown, -1)
// TODO:
//     E_TESSELLATION_CONTROL_SHADER
//     E_TESSELLATION_EVALUATION_SHADER
//     E_GEOMETRY_SHADER
//     E_MESH_SHADER
MJSON_ENUM_END(ShaderType)

MJSON_OBJECT_BEGIN(ShaderDescr, "Shader Description", nullptr)
    MJSON_FIELD(std::string, shader_name, "Shader Name", nullptr)
    MJSON_FIELD(ShaderType, shader_type, "Shader Type", nullptr)
MJSON_OBJECT_END(ShaderDescr)

MJSON_OBJECT_BEGIN(
        RenderConfig,
        "Render Configuration",
        "Configuration of render system")
    MJSON_FIELD(int, width, "Width", nullptr)
    MJSON_FIELD(int, height, "Height", nullptr)
    MJSON_FIELD(std::vector<ShaderDescr>, used_shaders,
                "Used shaders", "List of used shaders")
MJSON_OBJECT_END(RenderConfig)

// graphics output config
MJSON_ENUM_BEGIN(WindowMode, "Window Mode", nullptr)
    MJSON_ENUM_UNIT(E_REGULAR_WINDOW,    regular)
    MJSON_ENUM_UNIT(E_FRAMELESS_WINDOW,  frameless)
    MJSON_ENUM_UNIT(E_FULLSCREEN_WINDOW, fullscreen)
MJSON_ENUM_END(WindowMode)

MJSON_OBJECT_BEGIN(GraphicsOutputConfig, "Graphics Output Configuration", nullptr)
    MJSON_FIELD(WindowMode, mode, "Window Mode", nullptr)
    MJSON_FIELD(int, width, "Width", nullptr)
    MJSON_FIELD(int, height, "Height", nullptr)
    MJSON_FIELD(std::string, caption, "Caption", nullptr)
    MJSON_FIELD(bool, is_first_person, "Is First Person", nullptr)
MJSON_OBJECT_END(GraphicsOutputConfig)

// common config
MJSON_OBJECT_BEGIN(CommonConfig, "Common Configuration", nullptr)
    MJSON_FIELD(RenderType, render_type, "Render Type", nullptr)
MJSON_OBJECT_END(CommonConfig)

#ifndef QCE_CONFIGS_ANCILLARY_H
#define QCE_CONFIGS_ANCILLARY_H

namespace QCE {
    using ::RenderType, ::CameraType, ::ShaderType, ::HidEventType, ::WindowMode;
    using ::CameraConfigUnit, ::CameraConfig;
    using ::MovementConfig;
    using ::ShaderDescr, ::RenderConfig;
    using ::HidEventDescriptor, ::HidSingleEvent, ::HidConfig;
    using ::GraphicsOutputConfig, ::CommonConfig;

#  ifdef WIN32
    static constexpr auto DEFAULT_RENDER_TYPE = RenderType::E_RENDER_DIRECTX12;
#  else
    static constexpr auto DEFAULT_RENDER_TYPE = RenderType::E_RENDER_VULKAN;
#  endif

    static const CameraConfigUnit DEFAULT_CAMERA_CONFIG_UNIT = {
        .aspect  = (16.0f / 9.0f),
        .fov_rad = 1.0471975512f, // 60 deg
        .znear   = 1.0f,
        .zfar    = 1000.0f,

        .position = { 2.0f, 2.0f, -2.0f },
        .target   = { 0.0f, 0.0f, 0.0f },
        .up       = { 0.0f, 1.0f, 0.0f },

        .camera_name  = "MainCamera",
        .camera_type  = CameraType::E_CAMERA_FIRST_PERSON
    };

    static const CameraConfig DEFAULT_CAMERA_CONFIG = {
        .cameras = { DEFAULT_CAMERA_CONFIG_UNIT }
    };

    static const RenderConfig DEFAULT_RENDER_CONFIG = {
        .width = 1280,
        .height = 720,
        .used_shaders = {
            { "simple", QCE::ShaderType::E_VERTEX_SHADER },
            { "simple", QCE::ShaderType::E_PIXEL_SHADER }
        }
    };
}

#endif // QCE_CONFIGS_ANCILLARY_H
