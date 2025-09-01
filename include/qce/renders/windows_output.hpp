// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/renders/render_base.hpp>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace QCE {
    class WinNtWindow final {
    public:
        explicit WinNtWindow(
            GraphicsOutputConfig initial_config,
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
                    app->StepForward();
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
        ErrorCode Init();

        LRESULT MessageProcess(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        GraphicsOutputConfig m_config{};
        const std::wstring m_class_name;

        HWND m_hwnd = nullptr;
    };
}

