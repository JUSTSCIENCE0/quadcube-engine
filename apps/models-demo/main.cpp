// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef CU_BUILD_SPECIFIC_SIMD
#  define CU_BUILD_SPECIFIC_SIMD sse2
#endif // !CU_BUILD_SPECIFIC_SIMD

#include <qce/qce.hpp>

int main(int argc, char* argv[]) {
#ifdef NDEBUG
    FreeConsole();
#endif

    auto& app = QCE::Application<>::Get();
    QCE_CRITICAL(app.Setup<>());
    QCE_CRITICAL(app.LoadScene("models_demo"));
    return app.Run();
}
