// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/renders/render_dx12.hpp>

int main(int argc, char* argv[]) {
#ifdef NDEBUG
    FreeConsole();
#endif

    QCE::WindowConfig wconf{};
    QCE::WinNtWindow window{ wconf };

    std::getchar();

    return 0;
}