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
    class WinNtWindow {
    public:
        explicit WinNtWindow(
            WindowConfig initial_config,
            std::wstring class_name = L"QceMainWindow");
        WinNtWindow(const WinNtWindow&) = delete;
        WinNtWindow(WinNtWindow&&) = delete;
        WinNtWindow& operator=(const WinNtWindow&) = delete;
        WinNtWindow& operator=(WinNtWindow&&) = delete;
        ~WinNtWindow() = default;

        ErrorCode UpdateConfig(WindowConfig config);
        ErrorCode MainLoop();

    private:
        ErrorCode Init();

        LRESULT MessageProcess(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        WindowConfig m_config{};
        const std::wstring m_class_name;

        HWND m_hwnd = nullptr;
    };
}

