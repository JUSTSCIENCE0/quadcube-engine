// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// the engine objects have already been declared and defined within the engine
// do not generate here
#ifndef MJSON_OBJECTS_GENERATION_PART

// common
using QCERenderType = QCE::RenderType;
MJSON_ENUM_BEGIN(QCERenderType,
        "Render Type", "Rendering system API")
    MJSON_ENUM_UNIT(QCE::RenderType::E_RENDER_DIRECTX12, directx12)
    MJSON_ENUM_UNIT(QCE::RenderType::E_RENDER_VULKAN, vulkan)
    MJSON_ENUM_VALUE(QCE::RenderType::E_RENDER_UNKNOWN, unknown, -1)
MJSON_ENUM_END(QCERenderType)

// camera config
using QCECameraType = QCE::CameraType;
MJSON_ENUM_BEGIN(QCECameraType, "Camera Type", nullptr)
    MJSON_ENUM_UNIT(QCE::CameraType::E_CAMERA_FIXED, fixed)
    MJSON_ENUM_UNIT(QCE::CameraType::E_CAMERA_FIRST_PERSON, first_person)
    MJSON_ENUM_UNIT(QCE::CameraType::E_CAMERA_THIRD_PERSON, third_person)
MJSON_ENUM_END(QCECameraType)

MJSON_OBJECT_BEGIN(
        QCE::CameraConfigUnit,
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
    MJSON_FIELD(QCE::CameraType, camera_type, "Camera Type", nullptr)
MJSON_OBJECT_END(QCE::CameraConfigUnit)

MJSON_OBJECT_BEGIN(
        QCE::CameraConfig,
        "Camera Config",
        "Configuration of the camera system")
    MJSON_FIELD(std::vector<QCE::CameraConfigUnit>, cameras,
                "Cameras", "Description of all cameras")
MJSON_OBJECT_END(QCE::CameraConfig)

// hid config
using QCEHidEventType = QCE::HidEventType;
MJSON_ENUM_BEGIN(QCEHidEventType, "Hid Event Type", nullptr)
    MJSON_ENUM_UNIT (QCE::HidEventType::E_HET_SINGLE,  single)
    MJSON_ENUM_UNIT (QCE::HidEventType::E_HET_ACCORD,  accord)
    MJSON_ENUM_UNIT (QCE::HidEventType::E_HET_COMBO,   combo)
    MJSON_ENUM_VALUE(QCE::HidEventType::E_HET_UNKNOWN, unknown, -1)
MJSON_ENUM_END(QCEHidEventType)

#define MJSON_BASE_OBJECT_NAME QCE::HidEventDescriptor
MJSON_POLYMORPHIC_OBJECT_BEGIN(
        "HID Event Descriptor",
        "HID Event Descriptor Base Class")
    MJSON_BASE_OBJECT_BEGIN(QCE::HidEventType)
      MJSON_BASE_OBJECT_FIELD(std::string, handler,
        "Handler",
        "Handler name for this event")
    MJSON_BASE_OBJECT_END()
    MJSON_DERIVED_OBJECT_BEGIN(QCE::HidSingleEvent, QCE::HidEventType::E_HET_SINGLE)
      MJSON_DERIVED_OBJECT_FIELD(std::string, hid_event_code_name,
        "HID Event Code Name",
        "Name of the HID event code\n"
        "Structure: <INPUT_DEVICE>_<EVENT>")
    MJSON_DERIVED_OBJECT_END(QCE::HidSingleEvent)
    // TODO - HidAccordEvent, HidComboEvent
MJSON_POLYMORPHIC_OBJECT_END()
#undef MJSON_BASE_OBJECT_NAME

MJSON_OBJECT_BEGIN(
        QCE::HidConfig,
        "HID Config",
        "Configuration of the HID system")
    MJSON_FIELD(
        std::vector<std::unique_ptr<QCE::HidEventDescriptor>>,
        event_descriptors, nullptr, nullptr)
MJSON_OBJECT_END(QCE::HidConfig)

// movement config
MJSON_OBJECT_BEGIN(
        QCE::MovementConfig,
        "Movement Config",
        "Configuration of the movement system")
    MJSON_FIELD(std::optional<std::string>, temporal, nullptr, nullptr)
MJSON_OBJECT_END(QCE::MovementConfig)

// render config
using QCEShaderType = QCE::ShaderType;
MJSON_ENUM_BEGIN(QCEShaderType, "Shader Type", nullptr)
    MJSON_ENUM_UNIT (QCE::ShaderType::E_VERTEX_SHADER, vertex)
    MJSON_ENUM_UNIT (QCE::ShaderType::E_PIXEL_SHADER,  pixel)
// ancillary
    MJSON_ENUM_UNIT (QCE::ShaderType::E_SHADERS_TYPE_COUNT, count)
    MJSON_ENUM_VALUE(QCE::ShaderType::E_UNKNOWN_SHADER, unknown, -1)
// TODO:
//     E_TESSELLATION_CONTROL_SHADER
//     E_TESSELLATION_EVALUATION_SHADER
//     E_GEOMETRY_SHADER
//     E_MESH_SHADER
MJSON_ENUM_END(QCEShaderType)

MJSON_OBJECT_BEGIN(QCE::ShaderDescr, "Shader Description", nullptr)
    MJSON_FIELD(std::string, shader_name, "Shader Name", nullptr)
    MJSON_FIELD(QCE::ShaderType, shader_type, "Shader Type", nullptr)
MJSON_OBJECT_END(QCE::ShaderDescr)

MJSON_OBJECT_BEGIN(
        QCE::RenderConfig,
        "Render Configuration",
        "Configuration of render system")
    MJSON_FIELD(int, width, "Width", nullptr)
    MJSON_FIELD(int, height, "Height", nullptr)
    MJSON_FIELD(std::vector<QCE::ShaderDescr>, used_shaders,
                "Used shaders", "List of used shaders")
MJSON_OBJECT_END(QCE::RenderConfig)

// graphics output config
using QCEWindowMode = QCE::WindowMode;
MJSON_ENUM_BEGIN(QCEWindowMode, "Window Mode", nullptr)
    MJSON_ENUM_UNIT(QCE::WindowMode::E_REGULAR_WINDOW,    regular)
    MJSON_ENUM_UNIT(QCE::WindowMode::E_FRAMELESS_WINDOW,  frameless)
    MJSON_ENUM_UNIT(QCE::WindowMode::E_FULLSCREEN_WINDOW, fullscreen)
MJSON_ENUM_END(QCEWindowMode)

MJSON_OBJECT_BEGIN(QCE::GraphicsOutputConfig, "Graphics Output Configuration", nullptr)
    MJSON_FIELD(QCE::WindowMode, mode, "Window Mode", nullptr)
    MJSON_FIELD(int, width, "Width", nullptr)
    MJSON_FIELD(int, height, "Height", nullptr)
    MJSON_FIELD(std::string, caption, "Caption", nullptr)
    MJSON_FIELD(bool, is_first_person, "Is First Person", nullptr)
MJSON_OBJECT_END(QCE::GraphicsOutputConfig)

// common config
MJSON_OBJECT_BEGIN(QCE::CommonConfig, "Common Configuration", nullptr)
    MJSON_FIELD(QCE::RenderType, render_type, "Render Type", nullptr)
MJSON_OBJECT_END(QCE::CommonConfig)

#endif