// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/renders/windows_output.hpp>

int main(int argc, char* argv[]) {
#ifdef NDEBUG
    FreeConsole();
#endif

    try {
        QCE::WindowConfig wconf{};
        QCE::WinNtWindow window{ wconf };

        return window.MainLoop();
    }
    catch (QCE::ErrorCodeException qce_ex) {
        MessageBox(nullptr, L"QCE Exception", L"Error", 0);
        return qce_ex.code_value();
    }
    catch (std::exception ex) {
        MessageBox(nullptr, L"General Exception", L"Error", 0);
        return -1;
    }
    catch (...) {
        MessageBox(nullptr, L"Unknown Exception", L"Error", 0);
        return -1;
    }
}