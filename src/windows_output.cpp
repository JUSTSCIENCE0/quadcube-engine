// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/renders/windows_output.hpp>

namespace QCE {
    WinNtWindow::WinNtWindow(
                GraphicsOutputConfig initial_config,
                std::wstring class_name) :
            m_config(std::move(initial_config)),
            m_class_name(std::move(class_name)) {
        QCE_THROW_CRITICAL(Init());
    }

    ErrorCode WinNtWindow::UpdateConfig(GraphicsOutputConfig config) {
        m_config = std::move(config);
        return ErrorCode::SUCCESS;
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

        m_hwnd = CreateWindow(wc.lpszClassName, m_config.caption.c_str(),
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, m_config.width, m_config.height, nullptr, nullptr, wc.hInstance, nullptr);
        if (!m_hwnd) {
            return ErrorCode::E_WINNT_CREATE_FAILED;
        }

        SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        ShowWindow(m_hwnd, SW_SHOW);
        UpdateWindow(m_hwnd);

        return ErrorCode::SUCCESS;
    }

    LRESULT WinNtWindow::MessageProcess(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        assert(hwnd == m_hwnd);

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
            // TODO - handle
            return 0;
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
            // TODO - handle
            return 0;
        case WM_MOUSEMOVE:
            // TODO - handle
            return 0;
        case WM_KEYUP:
            // TODO - handle
            return 0;
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
