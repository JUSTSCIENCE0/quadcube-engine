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
    class WinNtWindow : public BaseWindow {
    public:
        explicit WinNtWindow(
            WindowConfig initial_config,
            std::wstring class_name = L"QceMainWindow");
        virtual ~WinNtWindow() = default;

    private:
        ErrorCode Init();

        LRESULT MessageProcess(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        const std::wstring m_class_name;
    };
}

