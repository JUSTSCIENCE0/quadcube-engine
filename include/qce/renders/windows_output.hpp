// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/renders/render_base.hpp>
#include <qce/hid/win_gamepad.hpp>
#include <qce/systems/hid_system.hpp>

#include <array>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace QCE {
    class WinNtWindow final {
    public:
        explicit WinNtWindow(
            GraphicsOutputConfig initial_config,
            HidSystem& hid_system,
            std::wstring class_name = L"QceMainWindow");
        WinNtWindow(const WinNtWindow&) = delete;
        WinNtWindow(WinNtWindow&&) = delete;
        WinNtWindow& operator=(const WinNtWindow&) = delete;
        WinNtWindow& operator=(WinNtWindow&&) = delete;
        ~WinNtWindow() = default;

        ErrorCode UpdateConfig(GraphicsOutputConfig config);

        template <typename /*TODO: concept*/ TickConsumer>
        ErrorCode MainLoop(TickConsumer* app) {
            MSG msg{};

            while (msg.message != WM_QUIT) {
                if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else {
                    GamepadsProcess();
                    QCE_CRITICAL(app->StepForward());
                }
            }

            return ErrorCode::SUCCESS;
        }

        void ShowMessage(
                const std::wstring& caption,
                const std::wstring& text) {
            MessageBox(nullptr, text.c_str(), caption.c_str(), 0);
        }

        HWND GetHwnd() const noexcept { return m_hwnd; }

    private:
        using clock = std::chrono::steady_clock;

        static constexpr auto GAMEPAD_UPDATE_INTERVAL_MS = 1; // 1000 Hz

        ErrorCode Init();

        LRESULT MessageProcess(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void GamepadsProcess() noexcept;
        void CenterCursor();

        GraphicsOutputConfig m_config{};
        const std::wstring m_class_name;

        HidSystem& m_hid_system;

        HWND m_hwnd = nullptr;

        bool  m_handle_next_mouse_move = false;
        float m_prev_mouse_x = 0.0f;
        float m_prev_mouse_y = 0.0f;

        std::array<XInputGamepad, XUSER_MAX_COUNT> m_gamepads{
            XInputGamepad(0, m_hid_system),
            XInputGamepad(1, m_hid_system),
            XInputGamepad(2, m_hid_system),
            XInputGamepad(3, m_hid_system)
        };
        clock::time_point m_prev_gamepad_update = clock::now();
    };
}
