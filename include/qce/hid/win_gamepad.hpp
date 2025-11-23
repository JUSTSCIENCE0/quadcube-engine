// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/hid/events.hpp>

#include <stdint.h>
#include <iostream>
#include <cmath>

#include <Xinput.h>

namespace QCE {
    class XInputGamepad final {
    public:
        explicit XInputGamepad(uint8_t id) noexcept :
            m_id(id) {}

        void Update() noexcept {
            XINPUT_STATE state{};
            DWORD res = XInputGetState(m_id, &state);
            if (res != ERROR_SUCCESS) {
                // gamepad offline
                if (m_is_connected) {
                    m_is_connected = false;
                    auto event = hid_event_on_gamepad_connection(m_id, false);
                    // TODO - handle
                    std::cout << hid_event_describe(event) << std::endl;
                }
                return;
            }

            if (!m_is_connected) {
                m_is_connected = true;
                auto event = hid_event_on_gamepad_connection(m_id, true);
                // TODO - handle
                std::cout << hid_event_describe(event) << std::endl;
            }

            const XINPUT_GAMEPAD& g = state.Gamepad;

            if (g.wButtons != m_prev_buttons_state) {
                auto buttons_diff = g.wButtons ^ m_prev_buttons_state;
#define HANDLE_GAMEPAD_BUTTON(ButtonConst, EventCode) \
                if (buttons_diff & ButtonConst) { \
                    auto event = (g.wButtons & ButtonConst) ? \
                        hid_event_on_button_down(EventCode, m_id) : \
                        hid_event_on_button_up(EventCode, m_id); \
                    /* TODO - handle */  \
                    std::cout << hid_event_describe(event) << std::endl; \
                }

                HANDLE_GAMEPAD_BUTTON(XINPUT_GAMEPAD_A, HidEventCode::E_HEC_GAMEPAD_RPAD_DOWN)
                HANDLE_GAMEPAD_BUTTON(XINPUT_GAMEPAD_B, HidEventCode::E_HEC_GAMEPAD_RPAD_RIGHT)
                HANDLE_GAMEPAD_BUTTON(XINPUT_GAMEPAD_X, HidEventCode::E_HEC_GAMEPAD_RPAD_LEFT)
                HANDLE_GAMEPAD_BUTTON(XINPUT_GAMEPAD_Y, HidEventCode::E_HEC_GAMEPAD_RPAD_UP)

                HANDLE_GAMEPAD_BUTTON(XINPUT_GAMEPAD_DPAD_UP,    HidEventCode::E_HEC_GAMEPAD_LPAD_UP)
                HANDLE_GAMEPAD_BUTTON(XINPUT_GAMEPAD_DPAD_RIGHT, HidEventCode::E_HEC_GAMEPAD_LPAD_RIGHT)
                HANDLE_GAMEPAD_BUTTON(XINPUT_GAMEPAD_DPAD_DOWN,  HidEventCode::E_HEC_GAMEPAD_LPAD_DOWN)
                HANDLE_GAMEPAD_BUTTON(XINPUT_GAMEPAD_DPAD_LEFT,  HidEventCode::E_HEC_GAMEPAD_LPAD_LEFT)

                HANDLE_GAMEPAD_BUTTON(XINPUT_GAMEPAD_START, HidEventCode::E_HEC_GAMEPAD_START)
                HANDLE_GAMEPAD_BUTTON(XINPUT_GAMEPAD_BACK,  HidEventCode::E_HEC_GAMEPAD_SELECT)

                HANDLE_GAMEPAD_BUTTON(XINPUT_GAMEPAD_LEFT_THUMB,  HidEventCode::E_HEC_GAMEPAD_LSTICK_BUTTON)
                HANDLE_GAMEPAD_BUTTON(XINPUT_GAMEPAD_RIGHT_THUMB, HidEventCode::E_HEC_GAMEPAD_RSTICK_BUTTON)

                HANDLE_GAMEPAD_BUTTON(XINPUT_GAMEPAD_LEFT_SHOULDER,  HidEventCode::E_HEC_GAMEPAD_LSHOULDER)
                HANDLE_GAMEPAD_BUTTON(XINPUT_GAMEPAD_RIGHT_SHOULDER, HidEventCode::E_HEC_GAMEPAD_RSHOULDER)

#undef HANDLE_GAMEPAD_BUTTON
            }
            m_prev_buttons_state = g.wButtons;

            if (g.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
                float intensity = float(g.bLeftTrigger) / 255.0f;
                auto event = hid_event_on_trigger(m_id, HidEventCode::E_HEC_GAMEPAD_LTRIGGER, intensity);
                // TODO - handle
                std::cout << hid_event_describe(event) << std::endl;
            }
            if (g.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
                float intensity = float(g.bRightTrigger) / 255.0f;
                auto event = hid_event_on_trigger(m_id, HidEventCode::E_HEC_GAMEPAD_RTRIGGER, intensity);
                // TODO - handle
                std::cout << hid_event_describe(event) << std::endl;
            }

            if ((std::abs(g.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ||
                (std::abs(g.sThumbLY) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) {
                float dx = float(g.sThumbLX) / 32767.0f;
                float dy = float(g.sThumbLY) / 32767.0f;
                auto event = hid_event_on_stick(m_id, HidEventCode::E_HEC_GAMEPAD_LSTICK_MOVE, dx, dy);
                // TODO - handle
                std::cout << hid_event_describe(event) << std::endl;
            }
            if ((std::abs(g.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ||
                (std::abs(g.sThumbRY) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)) {
                float dx = float(g.sThumbRX) / 32767.0f;
                float dy = float(g.sThumbRY) / 32767.0f;
                auto event = hid_event_on_stick(m_id, HidEventCode::E_HEC_GAMEPAD_RSTICK_MOVE, dx, dy);
                // TODO - handle
                std::cout << hid_event_describe(event) << std::endl;
            }
        }

    private:
        const uint8_t m_id;

        bool m_is_connected = false;
        WORD m_prev_buttons_state = 0;

        // TODO: event handler
    };
}
