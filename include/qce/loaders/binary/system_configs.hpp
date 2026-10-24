// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/systems/system_configs.hpp>

#include <bitsery/brief_syntax/optional.h>
#include <bitsery/brief_syntax/string.h>
#include <bitsery/brief_syntax/vector.h>

#include <cassert>
#include <limits>

namespace QCE {
    namespace binary_detail {
        template<typename S>
        void serialize_hid_event_fields(S& s, HidEventDescriptor& descriptor) {
            s.value1b(descriptor.type);
            s.object(descriptor.handler);
        }
    }

    template<typename S>
    void serialize(S& s, CameraConfigUnit& config) {
        s.value4b(config.aspect);
        s.value4b(config.fov_rad);
        s.value4b(config.znear);
        s.value4b(config.zfar);
        s.object(config.position);
        s.object(config.target);
        s.object(config.up);
        s.object(config.camera_name);
        s.value1b(config.camera_type);
    }

    template<typename S>
    void serialize(S& s, CameraConfig& config) {
        s.object(config.cameras);
    }

    template<typename S>
    void serialize(S& s, HidEventDescriptor& descriptor) {
        binary_detail::serialize_hid_event_fields(s, descriptor);
    }

    template<typename S>
    void serialize(S& s, HidSingleEvent& event) {
        s.object(static_cast<HidEventDescriptor&>(event));
        s.object(event.hid_event_code_name);
    }

    template<typename S>
    void serialize(S& s, HidConfig& config) {
        s.container(config.event_descriptors, std::numeric_limits<size_t>::max(),
            [](S& ser, std::unique_ptr<HidEventDescriptor>& descriptor) {
                auto type = descriptor
                    ? descriptor->type
                    : HidEventType::E_HET_UNKNOWN;
                ser.value1b(type);

                if (!descriptor) {
                    if (type == HidEventType::E_HET_SINGLE)
                        descriptor = std::make_unique<HidSingleEvent>();
                    else
                        descriptor = std::make_unique<HidEventDescriptor>();
                    descriptor->type = type;
                }

                ser.object(descriptor->handler);
                if (type == HidEventType::E_HET_SINGLE) {
                    auto* event = dynamic_cast<HidSingleEvent*>(descriptor.get());
                    assert(event);
                    ser.object(event->hid_event_code_name);
                }
            });
    }

    template<typename S>
    void serialize(S& s, MovementConfig& config) {
        s.object(config.temporal);
    }

    template<typename S>
    void serialize(S& s, ShaderDescr& description) {
        s.object(description.shader_name);
        s.value1b(description.shader_type);
    }

    template<typename S>
    void serialize(S& s, RenderConfig& config) {
        s.value4b(config.width);
        s.value4b(config.height);
        s.object(config.used_shaders);
    }

    template<typename S>
    void serialize(S& s, GraphicsOutputConfig& config) {
        s.value1b(config.mode);
        s.value4b(config.width);
        s.value4b(config.height);
        s.object(config.caption);
        s.value1b(config.is_first_person);
    }

    template<typename S>
    void serialize(S& s, CommonConfig& config) {
        s.value1b(config.render_type);
    }
}
