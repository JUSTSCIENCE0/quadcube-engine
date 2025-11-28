// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/command.hpp>

#include <array>
#include <cassert>
#include <string>
#include <sstream>
#include <chrono>

#include <stdint.h>

namespace QCE {
    static constexpr auto FUNC_KEYS_COUNT = 12;

    #define CU_ENUMS_DESCRIPTION \
        CU_BEGIN_ENUM(HidEventParamType) \
            CU_ENUM_UNIT(E_HEPT_NONE) \
            CU_ENUM_UNIT(E_HEPT_COORDINATES)  /* X, Y */ \
            CU_ENUM_UNIT(E_HEPT_INTENSITY)    /*  Val */ \
            CU_ENUM_UNIT(E_HEPT_DISPLACEMENT) /* X, Y */ \
        CU_END_ENUM(HidEventParamType) \
        CU_BEGIN_ENUM(HidEventCode) \
            /* free: 7...12, 119...127 */ \
                   CU_ENUM_UNIT(E_HEC_MOUSE_LB) \
                   CU_ENUM_UNIT(E_HEC_MOUSE_RB) \
                   CU_ENUM_UNIT(E_HEC_MOUSE_MB) \
                   CU_ENUM_UNIT(E_HEC_MOUSE_B4) \
                   CU_ENUM_UNIT(E_HEC_MOUSE_B5) \
                   CU_ENUM_UNIT(E_HEC_MOUSE_SCROLL) /* wo UP/DOWN */ \
                   CU_ENUM_UNIT(E_HEC_MOUSE_MOVE)   /* wo UP/DOWN */ \
            CU_VALUED_ENUM_UNIT(E_HEC_GAMEPAD_CONNECTED, 13) /* wo DOWN(1) - on, UP(0) - off */ \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_START) \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_SELECT) \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_LPAD_UP) \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_LPAD_RIGHT) \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_LPAD_DOWN) \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_LPAD_LEFT) \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_RPAD_UP) \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_RPAD_RIGHT) \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_RPAD_DOWN) \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_RPAD_LEFT) \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_LSHOULDER) \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_RSHOULDER) \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_LTRIGGER) /* wo UP/DOWN */ \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_RTRIGGER) /* wo UP/DOWN */ \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_LSTICK_BUTTON) \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_RSTICK_BUTTON) \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_LSTICK_MOVE) /* wo UP/DOWN */ \
                   CU_ENUM_UNIT(E_HEC_GAMEPAD_RSTICK_MOVE) /* wo UP/DOWN */ \
            CU_VALUED_ENUM_UNIT(E_HEC_KEYBOARD_SPACE, ' ') \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_ENTER) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_TILDE) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_ESC) \
            CU_VALUED_ENUM_UNIT(E_HEC_KEYBOARD_FUNC_BEGIN, 36) \
            CU_VALUED_ENUM_UNIT(E_HEC_KEYBOARD_FUNC_END, E_HEC_KEYBOARD_FUNC_BEGIN - 1 + FUNC_KEYS_COUNT) \
            CU_VALUED_ENUM_UNIT(E_HEC_KEYBOARD_NUMBER_BEGIN, '0') \
            CU_VALUED_ENUM_UNIT(E_HEC_KEYBOARD_NUMBER_END,   '9') \
            CU_VALUED_ENUM_UNIT(E_HEC_KEYBOARD_UP, 58) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_RIGHT) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_DOWN) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_LEFT) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_CTRL) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_SHIFT) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_CAPS_LOCK) \
            CU_VALUED_ENUM_UNIT(E_HEC_KEYBOARD_CHARS_BEGIN, 'A') \
            CU_VALUED_ENUM_UNIT(E_HEC_KEYBOARD_CHARS_END,   'Z') \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_NUMPAD_START) \
            CU_VALUED_ENUM_UNIT(E_HEC_KEYBOARD_NUMPAD_END, E_HEC_KEYBOARD_NUMPAD_START + ('9' - '0')) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_TAB) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_INSERT) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_HOME) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_PG_UP) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_PG_DN) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_END) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_DELETE) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_BACKSPACE) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_COMMA) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_DOT) \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_FORWARD_SLASH) /* /? */ \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_BACK_SLASH)    /* \| */ \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_SEMICOLON)     /* ;: */ \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_APOSTROPHE)    /* '" */ \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_OPEN_BRACKET)  /* [{ */ \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_CLOSE_BRACKET) /* ]} */ \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_MINUS)         /* -_ */ \
                   CU_ENUM_UNIT(E_HEC_KEYBOARD_PLUS)          /* =+ */ \
                   CU_ENUM_ANCILLARY_UNITS(E_HEC) \
        CU_END_ENUM(HidEventCode)
    #include <cu/enum-utils.hpp>
    #undef CU_ENUMS_DESCRIPTION

    static_assert(HidEventCode::E_HEC_COUNT < 128, "HidEventCode count must be less than 128");

    static inline constexpr auto hid_event_code_is_valid(HidEventCode code) noexcept {
        if (code >= 7 && code <= 12) {
            return false;
        }
        return (code >= 0) && (code < HidEventCode::E_HEC_COUNT);
    }

    /// Event parameter types and helpers

    static inline constexpr auto generate_event_param_types() noexcept {
        std::array<HidEventParamType, HidEventCode::E_HEC_COUNT> result{};
#define HID_EVENT_PARAM_TYPE(Code, ParamType) result[Code] = ParamType;

        for (int event = HidEventCode::E_HEC_MOUSE_LB; event <= HidEventCode::E_HEC_MOUSE_B5; event++) {
            HID_EVENT_PARAM_TYPE(event, HidEventParamType::E_HEPT_COORDINATES);
        }
        HID_EVENT_PARAM_TYPE(HidEventCode::E_HEC_MOUSE_SCROLL,        HidEventParamType::E_HEPT_INTENSITY);
        HID_EVENT_PARAM_TYPE(HidEventCode::E_HEC_MOUSE_MOVE,          HidEventParamType::E_HEPT_DISPLACEMENT);
        HID_EVENT_PARAM_TYPE(HidEventCode::E_HEC_GAMEPAD_LTRIGGER,    HidEventParamType::E_HEPT_INTENSITY);
        HID_EVENT_PARAM_TYPE(HidEventCode::E_HEC_GAMEPAD_RTRIGGER,    HidEventParamType::E_HEPT_INTENSITY);
        HID_EVENT_PARAM_TYPE(HidEventCode::E_HEC_GAMEPAD_LSTICK_MOVE, HidEventParamType::E_HEPT_DISPLACEMENT);
        HID_EVENT_PARAM_TYPE(HidEventCode::E_HEC_GAMEPAD_RSTICK_MOVE, HidEventParamType::E_HEPT_DISPLACEMENT);

#undef HID_EVENT_PARAM_TYPE

        return result;
    }

    static inline constexpr auto HID_EVENT_PARAM_TYPES = generate_event_param_types();

    static inline constexpr bool hid_event_has_parameters(HidEventCode code) noexcept {
        assert(hid_event_code_is_valid(code));
        return HID_EVENT_PARAM_TYPES[code] != HidEventParamType::E_HEPT_NONE;
    }

    static inline constexpr HidEventParamType hid_event_get_param_type(HidEventCode code) noexcept {
        assert(hid_event_code_is_valid(code));
        return HID_EVENT_PARAM_TYPES[code];
    }

    static inline constexpr bool hid_event_has_coordinates(HidEventCode code) noexcept {
        return HID_EVENT_PARAM_TYPES[code] == HidEventParamType::E_HEPT_COORDINATES;
    }

    static inline constexpr bool hid_event_has_intensity(HidEventCode code) noexcept {
        return HID_EVENT_PARAM_TYPES[code] == HidEventParamType::E_HEPT_INTENSITY;
    }

    static inline constexpr bool hid_event_has_displacement(HidEventCode code) noexcept {
        return HID_EVENT_PARAM_TYPES[code] == HidEventParamType::E_HEPT_DISPLACEMENT;
    }

    /// Event code type checkers and helpers

    static inline constexpr bool hid_event_is_button(HidEventCode code) noexcept {
        assert(hid_event_code_is_valid(code));

        if ((code == HidEventCode::E_HEC_MOUSE_SCROLL) ||
            (code == HidEventCode::E_HEC_MOUSE_MOVE)   ||
            (code == HidEventCode::E_HEC_GAMEPAD_CONNECTED) ||
            (code == HidEventCode::E_HEC_GAMEPAD_LTRIGGER) ||
            (code == HidEventCode::E_HEC_GAMEPAD_RTRIGGER) ||
            (code == HidEventCode::E_HEC_GAMEPAD_LSTICK_MOVE) ||
            (code == HidEventCode::E_HEC_GAMEPAD_RSTICK_MOVE)) {
            return false;
        }

        return true;
    }

    static inline constexpr bool hid_event_is_function_key(HidEventCode code) noexcept {
        return (code >= HidEventCode::E_HEC_KEYBOARD_FUNC_BEGIN) &&
               (code <= HidEventCode::E_HEC_KEYBOARD_FUNC_END);
    }

    static inline constexpr bool hid_event_is_char_key(HidEventCode code) noexcept {
        return ((code >= HidEventCode::E_HEC_KEYBOARD_NUMBER_BEGIN) &&
                (code <= HidEventCode::E_HEC_KEYBOARD_NUMBER_END)) ||
               ((code >= HidEventCode::E_HEC_KEYBOARD_CHARS_BEGIN) &&
                (code <= HidEventCode::E_HEC_KEYBOARD_CHARS_END));
    }

    static inline constexpr bool hid_event_is_numpad_key(HidEventCode code) noexcept {
        return (code >= HidEventCode::E_HEC_KEYBOARD_NUMPAD_START) &&
               (code <= HidEventCode::E_HEC_KEYBOARD_NUMPAD_END);
    }

    static inline constexpr bool hid_event_is_keyboard(HidEventCode code) noexcept {
        return (code >= HidEventCode::E_HEC_KEYBOARD_SPACE) &&
               (code <= HidEventCode::E_HEC_KEYBOARD_PLUS);
    }

    static inline constexpr bool hid_event_is_mouse(HidEventCode code) noexcept {
        return (code >= HidEventCode::E_HEC_MOUSE_LB) &&
               (code <= HidEventCode::E_HEC_MOUSE_MOVE);
    }

    static inline constexpr bool hid_event_is_gamepad(HidEventCode code) noexcept {
        return (code >= HidEventCode::E_HEC_GAMEPAD_CONNECTED) &&
               (code <= HidEventCode::E_HEC_GAMEPAD_RSTICK_MOVE);
    }

    static inline constexpr std::string hid_event_describe(HidEventCode code) noexcept {
        std::string result = "HID Type: ";
        if (hid_event_is_keyboard(code)) {
            result += "KEYBOARD, key: ";

            assert(hid_event_is_button(code));

            if (hid_event_is_function_key(code)) {
                int f_num = code - HidEventCode::E_HEC_KEYBOARD_FUNC_BEGIN + 1;
                result += "F" + std::to_string(f_num);
            }
            else if (hid_event_is_char_key(code)) {
                result += char(code);
            }
            else if (hid_event_is_numpad_key(code)) {
                result += "NUMPAD " + 
                    std::to_string(code - E_HEC_KEYBOARD_NUMPAD_START);
            }
            else {
                auto key_name = to_string(code) + sizeof("E_HEC_KEYBOARD");
                result += key_name;
            }
        }
        else if (hid_event_is_mouse(code)) {
            result += "MOUSE, ";

            if (hid_event_is_button(code)) {
                result += "key: ";
            }
            auto key_name = to_string(code) + sizeof("E_HEC_MOUSE");
            result += key_name;
        }
        else if (hid_event_is_gamepad(code)) {
            result += "GAMEPAD, ";
            if (hid_event_is_button(code)) {
                result += "key: ";
            }
            auto key_name = to_string(code) + sizeof("E_HEC_GAMEPAD");
            result += key_name;
        }
        else {
            assert(!"Invalid device type");
            return "Invalid device type";
        }

        return result;
    }

    /// HidEvent structure and helpers

    struct HidEvent : CommandContext {
        HidEvent(int32_t descr = 255, float p1 = 0.0f, float p2 = 0.0f):
            CommandContext(CommandContextType::E_CCT_HID_EVENT),
            descriptor(descr),
            param1(p1),
            param2(p2) {}

        using clock = std::chrono::steady_clock;

        static constexpr int32_t CODE_MASK      = 0x0000007F;
        static constexpr int32_t IS_BUTTON_MASK = 0x00000100;
        static constexpr int32_t IS_DOWN_MASK   = 0x00000080;
        static constexpr int32_t DEVICE_ID_MASK = 0x0000F000;

        static constexpr int DEVICE_ID_OFFSET = 12;

        int32_t descriptor = 255; // 0..6 - HidEventCode
                                  // 8 - is button, 7 - is button up (0) or down (1)
                                  // 12..15 - device id

        float    param1 = 0.0f;
        float    param2 = 0.0f;

        clock::time_point timestamp = clock::now();

        constexpr HidEventCode GetCode() const noexcept {
            return HidEventCode(descriptor & CODE_MASK);
        }
        constexpr int32_t GetDeviceId() const noexcept {
            return (descriptor & DEVICE_ID_MASK) >> DEVICE_ID_OFFSET;
        }
    };

    static inline HidEvent hid_event_on_button_up(HidEventCode code, uint8_t device_id = 0) noexcept {
        assert(hid_event_is_button(code));
        assert(!hid_event_is_mouse(code));
        assert(HID_EVENT_PARAM_TYPES[code] == HidEventParamType::E_HEPT_NONE);

        int event_descriptor = code | HidEvent::IS_BUTTON_MASK;
        if (hid_event_is_gamepad(code)) {
            assert(device_id < 16);
            event_descriptor |= (device_id << HidEvent::DEVICE_ID_OFFSET);
        }

        return { event_descriptor };
    }

    static inline HidEvent hid_event_on_button_down(HidEventCode code, uint8_t device_id = 0) noexcept {
        assert(hid_event_is_button(code));
        assert(!hid_event_is_mouse(code));
        assert(HID_EVENT_PARAM_TYPES[code] == HidEventParamType::E_HEPT_NONE);

        int event_descriptor = code | HidEvent::IS_BUTTON_MASK | HidEvent::IS_DOWN_MASK;
        if (hid_event_is_gamepad(code)) {
            assert(device_id < 16);
            event_descriptor |= (device_id << HidEvent::DEVICE_ID_OFFSET);
        }

        return { event_descriptor };
    }

    static inline HidEvent hid_event_on_mouse_button_up(HidEventCode code, float x, float y) noexcept {
        assert(hid_event_is_button(code));
        assert(hid_event_is_mouse(code));
        assert(HID_EVENT_PARAM_TYPES[code] == HidEventParamType::E_HEPT_COORDINATES);
        return { code | HidEvent::IS_BUTTON_MASK, x, y };
    }

    static inline HidEvent hid_event_on_mouse_button_down(HidEventCode code, float x, float y) noexcept {
        assert(hid_event_is_button(code));
        assert(hid_event_is_mouse(code));
        assert(HID_EVENT_PARAM_TYPES[code] == HidEventParamType::E_HEPT_COORDINATES);
        return { code | HidEvent::IS_BUTTON_MASK | HidEvent::IS_DOWN_MASK, x, y };
    }

    static inline HidEvent hid_event_on_scroll(float intensity) noexcept {
        static_assert(
            HID_EVENT_PARAM_TYPES[HidEventCode::E_HEC_MOUSE_SCROLL] == HidEventParamType::E_HEPT_INTENSITY,
            "Mouse scroll have to include intensity");

        assert(intensity >= -1.0f && intensity <= 1.0f);
        return { HidEventCode::E_HEC_MOUSE_SCROLL, intensity };
    }

    static inline HidEvent hid_event_on_mouse_move(float delta_x, float delta_y) noexcept {
        static_assert(
            HID_EVENT_PARAM_TYPES[HidEventCode::E_HEC_MOUSE_MOVE] == HidEventParamType::E_HEPT_DISPLACEMENT,
            "Mouse move have to include displacement");

        return { HidEventCode::E_HEC_MOUSE_MOVE, delta_x, delta_y };
    }

    static inline HidEvent hid_event_on_gamepad_connection(uint8_t device_id, bool is_connected) noexcept {
        static_assert(
            HID_EVENT_PARAM_TYPES[HidEventCode::E_HEC_GAMEPAD_CONNECTED] == HidEventParamType::E_HEPT_NONE,
            "Gamepad (dis)connection doesn't have any params");

        return { 
            HidEventCode::E_HEC_GAMEPAD_CONNECTED |
            (device_id << HidEvent::DEVICE_ID_OFFSET) |
            (is_connected ? HidEvent::IS_DOWN_MASK : 0)
        };
    }

    static inline HidEvent hid_event_on_trigger(
            uint8_t device_id, HidEventCode code, float intensity) noexcept {
        assert((code == HidEventCode::E_HEC_GAMEPAD_LTRIGGER) || (code == HidEventCode::E_HEC_GAMEPAD_RTRIGGER));
        assert(intensity >= 0.0f && intensity <= 1.0f);
        assert(HID_EVENT_PARAM_TYPES[code] == HidEventParamType::E_HEPT_INTENSITY);

        return { code | (device_id << HidEvent::DEVICE_ID_OFFSET), intensity };
    }

    static inline HidEvent hid_event_on_stick(
            uint8_t device_id, HidEventCode code, float delta_x, float delta_y) noexcept {
        assert((code == HidEventCode::E_HEC_GAMEPAD_LSTICK_MOVE) || (code == HidEventCode::E_HEC_GAMEPAD_RSTICK_MOVE));
        assert(HID_EVENT_PARAM_TYPES[code] == HidEventParamType::E_HEPT_DISPLACEMENT);

        return { code | (device_id << HidEvent::DEVICE_ID_OFFSET), delta_x, delta_y };
    }

    static inline std::string hid_event_describe(const HidEvent& hid_event) noexcept {
        auto code = hid_event.GetCode();
        bool is_button = hid_event.descriptor & HidEvent::IS_BUTTON_MASK;
        assert(is_button == hid_event_is_button(code));

        std::string result = std::to_string(hid_event.timestamp.time_since_epoch().count()) + " - ";
        if (hid_event_is_gamepad(code)) {
            auto device_id = hid_event.GetDeviceId();
            result += "Device ID: " + std::to_string(device_id) + ", ";
        }

        result += hid_event_describe(code);
        if (is_button) {
            bool is_down = hid_event.descriptor & HidEvent::IS_DOWN_MASK;
            result += is_down ? " DOWN" : " UP";
        }
        if (hid_event_has_coordinates(code)) {
            result += " coordinates: " +
                std::to_string(hid_event.param1) + ", " +
                std::to_string(hid_event.param2);
        }
        if (hid_event_has_intensity(code)) {
            result += " intensity: " + std::to_string(hid_event.param1);
        }
        if (hid_event_has_displacement(code)) {
            result += " displacement: " +
                std::to_string(hid_event.param1) + ", " +
                std::to_string(hid_event.param2);
        }

        if (HidEventCode::E_HEC_GAMEPAD_CONNECTED == code) {
            bool is_down = hid_event.descriptor & HidEvent::IS_DOWN_MASK;
            result += is_down ? " connected" : " disconnected";
        }

        return result;
    }
}
