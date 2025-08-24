// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/renders/render_base.hpp>

#include <windows.h>

int main(int argc, char* argv[]) {
#ifdef NDEBUG
    FreeConsole();
#endif

    MessageBoxA(NULL, "Hello QuadCube Engine!", "QCE Game Demo", MB_OK);
    return 0;
}