// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/components/camera.hpp>
#include <qce/renders/render_type.hpp>

#include <string>
#include <vector>

namespace QCE {
    struct CameraConfig {
        struct Unit {
            float aspect  = (16.0f / 9.0f);
            float fov_rad = deg_to_rad(60);
            float znear   = 1.0f;
            float zfar    = 1000.0f;

            std::vector<float> position = { 2.0f, 2.0f, -2.0f };
            std::vector<float> target   = { 0.0f, 0.0f, 0.0f };
            std::vector<float> up       = { 0.0f, 1.0f, 0.0f };

            std::string camera_name = "MainCamera";
            CameraType camera_type  = E_CAMERA_FIRST_PERSON;
        };

        RenderType render_type = DEFAULT_RENDER_TYPE;
        std::vector<Unit> cameras;
    };
}
