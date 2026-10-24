// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <stdint.h>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM_TYPED(RenderType, int8_t) \
        CU_ENUM_UNIT(E_RENDER_DIRECTX12) \
        CU_ENUM_UNIT(E_RENDER_VULKAN) \
        CU_VALUED_ENUM_UNIT(E_RENDER_UNKNOWN, -1) \
    CU_END_ENUM(RenderType) \
    CU_BEGIN_ENUM_TYPED(CameraType, int8_t) \
        CU_ENUM_UNIT(E_CAMERA_FIXED) \
        CU_ENUM_UNIT(E_CAMERA_FIRST_PERSON) \
        CU_ENUM_UNIT(E_CAMERA_THIRD_PERSON) \
    CU_END_ENUM(CameraType) \
    CU_BEGIN_ENUM_TYPED(HidEventType, int8_t) \
        CU_ENUM_UNIT(E_HET_SINGLE) \
        CU_ENUM_UNIT(E_HET_ACCORD) \
        CU_ENUM_UNIT(E_HET_COMBO) \
        CU_VALUED_ENUM_UNIT(E_HET_UNKNOWN, -1) \
    CU_END_ENUM(HidEventType) \
    CU_BEGIN_ENUM_TYPED(ShaderType, int8_t) \
        CU_ENUM_UNIT(E_VERTEX_SHADER) \
        CU_ENUM_UNIT(E_PIXEL_SHADER) \
        CU_VALUED_ENUM_UNIT(E_SHADERS_TYPE_COUNT, 2) \
        CU_VALUED_ENUM_UNIT(E_UNKNOWN_SHADER, -1) \
    CU_END_ENUM(ShaderType) \
    CU_BEGIN_ENUM_TYPED(WindowMode, int8_t) \
        CU_ENUM_UNIT(E_REGULAR_WINDOW) \
        CU_ENUM_UNIT(E_FRAMELESS_WINDOW) \
        CU_ENUM_UNIT(E_FULLSCREEN_WINDOW) \
    CU_END_ENUM(WindowMode)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    struct CameraConfigUnit {
        float aspect = 0.0f;
        float fov_rad = 0.0f;
        float znear = 0.0f;
        float zfar = 0.0f;

        std::vector<float> position{};
        std::vector<float> target{};
        std::vector<float> up{};

        std::string camera_name{};
        CameraType camera_type = CameraType::E_CAMERA_FIRST_PERSON;
    };

    struct CameraConfig {
        std::vector<CameraConfigUnit> cameras{};
    };

    struct HidEventDescriptor {
        HidEventType type = HidEventType::E_HET_UNKNOWN;
        std::string handler{};

        virtual ~HidEventDescriptor() = default;
    };

    struct HidSingleEvent : public HidEventDescriptor {
        std::string hid_event_code_name{};

        HidSingleEvent() {
            type = HidEventType::E_HET_SINGLE;
        }

    };

    struct HidConfig {
        std::vector<std::unique_ptr<HidEventDescriptor>> event_descriptors{};
    };

    struct MovementConfig {
        std::optional<std::string> temporal{};
    };

    struct ShaderDescr {
        std::string shader_name{};
        ShaderType shader_type = ShaderType::E_UNKNOWN_SHADER;
    };

    struct RenderConfig {
        int width = 0;
        int height = 0;
        std::vector<ShaderDescr> used_shaders{};
    };

    struct GraphicsOutputConfig {
        WindowMode mode = WindowMode::E_REGULAR_WINDOW;
        int width = 0;
        int height = 0;
        std::string caption{};
        bool is_first_person = false;
    };

    struct CommonConfig {
        RenderType render_type = RenderType::E_RENDER_UNKNOWN;
    };

#ifdef WIN32
    static constexpr auto DEFAULT_RENDER_TYPE = RenderType::E_RENDER_DIRECTX12;
#else
    static constexpr auto DEFAULT_RENDER_TYPE = RenderType::E_RENDER_VULKAN;
#endif

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