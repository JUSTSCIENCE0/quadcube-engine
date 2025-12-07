// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/hid/events.hpp>

#include <WinUser.h>

#include <optional>

namespace QCE {
    static inline std::optional<HidEventCode> hid_event_code_from_win_event(WPARAM win_event_code) noexcept {
        // Handle char keys
        if (((win_event_code >= '0') && (win_event_code <= '9')) ||
            ((win_event_code >= 'A') && (win_event_code <= 'Z'))) {
            return HidEventCode(win_event_code);
        }

        switch (win_event_code) {
            // Mouse buttons
            case VK_LBUTTON:
                return HidEventCode::E_HEC_MOUSE_LB;
            case VK_RBUTTON:
                return HidEventCode::E_HEC_MOUSE_RB;
            case VK_MBUTTON:
                return HidEventCode::E_HEC_MOUSE_MB;
            case VK_XBUTTON1:
                return HidEventCode::E_HEC_MOUSE_B4;
            case VK_XBUTTON2:
                return HidEventCode::E_HEC_MOUSE_B5;
            // Gamepad buttons
            // TODO
            // Keyboard keys
            case VK_SPACE:
                return HidEventCode::E_HEC_KEYBOARD_SPACE;
            case VK_RETURN:
                return HidEventCode::E_HEC_KEYBOARD_ENTER;
            case VK_OEM_3:
                return HidEventCode::E_HEC_KEYBOARD_TILDE;
            case VK_ESCAPE:
                return HidEventCode::E_HEC_KEYBOARD_ESC;
            case VK_UP:
                return HidEventCode::E_HEC_KEYBOARD_UP;
            case VK_RIGHT:
                return HidEventCode::E_HEC_KEYBOARD_RIGHT;
            case VK_DOWN:
                return HidEventCode::E_HEC_KEYBOARD_DOWN;
            case VK_LEFT:
                return HidEventCode::E_HEC_KEYBOARD_LEFT;
            case VK_CONTROL:
                return HidEventCode::E_HEC_KEYBOARD_CTRL;
            case VK_SHIFT:
                return HidEventCode::E_HEC_KEYBOARD_SHIFT;
            case VK_CAPITAL:
                return HidEventCode::E_HEC_KEYBOARD_CAPS_LOCK;
            case VK_TAB:
                return HidEventCode::E_HEC_KEYBOARD_TAB;
            case VK_INSERT:
                return HidEventCode::E_HEC_KEYBOARD_INSERT;
            case VK_HOME:
                return HidEventCode::E_HEC_KEYBOARD_HOME;
            case VK_PRIOR:
                return HidEventCode::E_HEC_KEYBOARD_PG_UP;
            case VK_NEXT:
                return HidEventCode::E_HEC_KEYBOARD_PG_DN;
            case VK_END:
                return HidEventCode::E_HEC_KEYBOARD_END;
            case VK_DELETE:
                return HidEventCode::E_HEC_KEYBOARD_DELETE;
            case VK_BACK:
                return HidEventCode::E_HEC_KEYBOARD_BACKSPACE;
            case VK_OEM_COMMA:
            case VK_DECIMAL:
                return HidEventCode::E_HEC_KEYBOARD_COMMA;
            case VK_OEM_PERIOD:
                return HidEventCode::E_HEC_KEYBOARD_DOT;
            case VK_OEM_2:
            case VK_DIVIDE:
                return HidEventCode::E_HEC_KEYBOARD_FORWARD_SLASH;
            case VK_OEM_5:
                return HidEventCode::E_HEC_KEYBOARD_BACK_SLASH;
            case VK_OEM_1:
                return HidEventCode::E_HEC_KEYBOARD_SEMICOLON;
            case VK_OEM_7:
                return HidEventCode::E_HEC_KEYBOARD_APOSTROPHE;
            case VK_OEM_4:
                return HidEventCode::E_HEC_KEYBOARD_OPEN_BRACKET;
            case VK_OEM_6:
                return HidEventCode::E_HEC_KEYBOARD_CLOSE_BRACKET;
            case VK_OEM_MINUS:
            case VK_SUBTRACT:
                return HidEventCode::E_HEC_KEYBOARD_MINUS;
            case VK_OEM_PLUS:
            case VK_ADD:
                return HidEventCode::E_HEC_KEYBOARD_PLUS;
            default:
                break;
        }

        // Handle numpad keys
        if ((win_event_code >= VK_NUMPAD0) && (win_event_code <= VK_NUMPAD9)) {
            return HidEventCode(E_HEC_KEYBOARD_NUMPAD_START + (win_event_code - VK_NUMPAD0));
        }
        // Handle functional keys
        if ((win_event_code >= VK_F1) && (win_event_code <= VK_F12)) {
            return HidEventCode(E_HEC_KEYBOARD_FUNC_BEGIN + (win_event_code - VK_F1));
        }
        return std::nullopt;
    }

    static inline constexpr std::optional<HidEventCode>
            hid_event_code_from_win_mouse_button(UINT msg, WPARAM wp) noexcept {
        switch (msg) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            return HidEventCode::E_HEC_MOUSE_LB;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            return HidEventCode::E_HEC_MOUSE_RB;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            return HidEventCode::E_HEC_MOUSE_MB;
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            if (GET_XBUTTON_WPARAM(wp) == XBUTTON1) {
                return HidEventCode::E_HEC_MOUSE_B4;
            }
            else if (GET_XBUTTON_WPARAM(wp) == XBUTTON2) {
                return HidEventCode::E_HEC_MOUSE_B5;
            }
            else {
                return std::nullopt;
            }
        default:
            return std::nullopt;
        }
    }
}
