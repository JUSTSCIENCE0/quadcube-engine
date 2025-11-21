// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <array>

namespace QCE {
    /*
    Events: 
        Button Down
        Button Up
        Wheel Scroll
        Mouse Move
        Trigger Press
        Stick Move
    */

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
            CU_VALUED_ENUM_UNIT(E_HEC_GAMEPAD_LOGO, 13) \
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

    static inline constexpr auto generate_event_param_types() {
        std::array<HidEventParamType, HidEventCode::E_HEC_COUNT> result{};
#define HID_EVENT_PARAM_TYPE(Code, ParamType) result[Code] = ParamType;

        for (int event = HidEventCode::E_HEC_MOUSE_LB; event <= HidEventCode::E_HEC_MOUSE_B5; event++) {
            HID_EVENT_PARAM_TYPE(event, HidEventParamType::E_HEPT_COORDINATES);
        }
        HID_EVENT_PARAM_TYPE(HidEventCode::E_HEC_MOUSE_SCROLL, HidEventParamType::E_HEPT_INTENSITY);
        HID_EVENT_PARAM_TYPE(HidEventCode::E_HEC_MOUSE_MOVE, HidEventParamType::E_HEPT_DISPLACEMENT);
        HID_EVENT_PARAM_TYPE(HidEventCode::E_HEC_GAMEPAD_LTRIGGER, HidEventParamType::E_HEPT_INTENSITY);
        HID_EVENT_PARAM_TYPE(HidEventCode::E_HEC_GAMEPAD_RTRIGGER, HidEventParamType::E_HEPT_INTENSITY);
        HID_EVENT_PARAM_TYPE(HidEventCode::E_HEC_GAMEPAD_LSTICK_MOVE, HidEventParamType::E_HEPT_DISPLACEMENT);
        HID_EVENT_PARAM_TYPE(HidEventCode::E_HEC_GAMEPAD_RSTICK_MOVE, HidEventParamType::E_HEPT_DISPLACEMENT);

#undef HID_EVENT_PARAM_TYPE

        return result;
    }

    static inline constexpr auto HID_EVENT_PARAM_TYPES = generate_event_param_types();
}
