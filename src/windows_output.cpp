// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/renders/windows_output.hpp>
#include <qce/hid/win_events.hpp>
#include <qce/ancillary/directories.hpp>

#include <cu/string-utils.hpp>

#include <windowsx.h>

namespace QCE {
    WinNtWindow::WinNtWindow(
                HidSystem& hid_system,
                std::wstring class_name) :
            m_hid_system(hid_system),
            m_class_name(std::move(class_name)) {}

    ErrorCode WinNtWindow::Setup() {
        const auto CONFIGS_DIR = QCE::get_configs_directory();
        const auto graphics_output_config_json_file = CONFIGS_DIR / "graphics_output_config.json";

        std::string error_descr = "";
        auto parse_result = macrojson::json_file_to_object(graphics_output_config_json_file, m_config, error_descr);
        if (macrojson::E_MJSON_OK != parse_result) {
            // TODO: use log system
            std::cout << error_descr << std::endl;
            return ErrorCode::E_ENG_BAD_CONFIG_FILE;
        }

        return Init();
    }

    ErrorCode WinNtWindow::Init() {
        WNDCLASS wc;
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION); // TODO: configurable
        wc.hCursor = LoadCursor(0, IDC_ARROW);   // TODO: hide cursor
        wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
        wc.lpszMenuName = 0;
        wc.lpszClassName = m_class_name.c_str();
        wc.hInstance = GetModuleHandle(nullptr);

        auto wnd_proc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
                auto wnd = reinterpret_cast<WinNtWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

                if (wnd) {
                    return wnd->MessageProcess(hwnd, msg, wParam, lParam);
                }
                return DefWindowProc(hwnd, msg, wParam, lParam);
            };
        wc.lpfnWndProc = wnd_proc;

        if (!RegisterClass(&wc)) {
            return ErrorCode::E_WINNT_REGISRER_FAILED;
        }

        RECT R = { 0, 0, m_config.width, m_config.height };
        if (!AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false)) {
            return ErrorCode::E_WINNT_INVALID_WINDOW_RECT;
        }

        auto w_caption = CU::str_to_wstr_simple(m_config.caption);
        m_hwnd = CreateWindow(wc.lpszClassName, w_caption.c_str(),
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, m_config.width, m_config.height, nullptr, nullptr, wc.hInstance, nullptr);
        if (!m_hwnd) {
            return ErrorCode::E_WINNT_CREATE_FAILED;
        }

        SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        ShowWindow(m_hwnd, SW_SHOW);
        UpdateWindow(m_hwnd);

        if (m_config.is_first_person)
            ShowCursor(FALSE);

        return ErrorCode::SUCCESS;
    }

    LRESULT WinNtWindow::MessageProcess(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        assert(hwnd == m_hwnd);

        auto get_mouse_position = [this](LPARAM lp) noexcept -> std::pair<float, float> {
            float x = float(GET_X_LPARAM(lp)) / float(m_config.width);
            float y = 1.0f - (float(GET_Y_LPARAM(lp)) / float(m_config.height));
            return { x, y };
        };

        switch (msg) {
        case WM_ACTIVATE:
            // WM_ACTIVATE is sent when the window is activated or deactivated
            // TODO - handle
            return 0;
        case WM_SIZE:
            // WM_SIZE is sent when the user resizes the window.
            // TODO - handle
            return 0;
        case WM_ENTERSIZEMOVE:
            // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
            return 0;
        case WM_EXITSIZEMOVE:
            // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
            // TODO - handle
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_XBUTTONDOWN:
        {
            auto event_code = hid_event_code_from_win_mouse_button(msg, wParam);
            if (!event_code.has_value()) {
                // TODO: use log system
                std::cout << "WM_BUTTONDOWN: skip unknown button code 0x" <<
                     std::hex << msg << " , param 0x" << wParam << std::dec << std::endl;
                break; // def win proc
            }
            auto [x, y] = get_mouse_position(lParam);
            m_hid_system.PushEvent(
                hid_event_on_mouse_button_down(event_code.value(), x, y));
            return 0;
        }
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        case WM_XBUTTONUP:
        {
            auto event_code = hid_event_code_from_win_mouse_button(msg, wParam);
            if (!event_code.has_value()) {
                // TODO: use log system
                std::cout << "WM_BUTTONUP: skip unknown button code 0x" <<
                    std::hex << msg << " , param 0x" << wParam << std::dec << std::endl;
                break; // def win proc
            }
            auto [x, y] = get_mouse_position(lParam);
            m_hid_system.PushEvent(
                hid_event_on_mouse_button_up(event_code.value(), x, y));
            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            float intensity = float(zDelta) / float(WHEEL_DELTA);
            m_hid_system.PushEvent(hid_event_on_scroll(intensity));
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            // TODO - check do we need this handler and early exit if we don't

            auto [x, y] = get_mouse_position(lParam);
            float dx = x - m_prev_mouse_x;
            float dy = y - m_prev_mouse_y;
            m_prev_mouse_x = x;
            m_prev_mouse_y = y;

            if (!m_handle_next_mouse_move) {
                m_handle_next_mouse_move = true;
                return 0;
            }

            m_hid_system.PushMouseMoveEvent(hid_event_on_mouse_move(dx, dy));

            if (m_config.is_first_person)
                CenterCursor();
            return 0;
        }
        case WM_KEYDOWN:
        {
            if (HIWORD(lParam) & KF_REPEAT)
                return 0; // ignore repeated signals from a pressed key
            auto event_code = hid_event_code_from_win_event(wParam);
            if (!event_code.has_value()) {
                // TODO: use log system
                std::cout << "WM_KEYDOWN: skip unknown key 0x" <<
                    std::hex << wParam << std::dec << std::endl;
                break; // def win proc
            }
            m_hid_system.PushEvent(
                hid_event_on_button_down(event_code.value()));
            return 0;
        }
        case WM_KEYUP:
        {
            auto event_code = hid_event_code_from_win_event(wParam);
            if (!event_code.has_value()) {
                // TODO: use log system
                std::cout << "WM_KEYUP: skip unknown key 0x" <<
                    std::hex << wParam << std::dec << std::endl;
                break; // def win proc
            }
            m_hid_system.PushEvent(
                hid_event_on_button_up(event_code.value()));

            return 0;
        }
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    void WinNtWindow::GamepadsProcess() noexcept {
        // TODO - check do we need this handler and early exit if we don't

        auto now = clock::now();
        if ((now - m_prev_gamepad_update) < std::chrono::milliseconds(GAMEPAD_UPDATE_INTERVAL_MS)) {
            return;
        }
        m_prev_gamepad_update = now;

        for (auto& gamepad : m_gamepads) {
            gamepad.Update();
        }
    }

    void WinNtWindow::CenterCursor() {
        POINT pt{
            .x = LONG(m_config.width / 2),
            .y = LONG(m_config.height / 2)
        };
        ClientToScreen(m_hwnd, &pt);
        SetCursorPos(pt.x, pt.y);
        m_handle_next_mouse_move = false;
    }
}
