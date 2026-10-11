// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef CU_BUILD_SPECIFIC_SIMD
#  define CU_BUILD_SPECIFIC_SIMD sse2
#endif // !CU_BUILD_SPECIFIC_SIMD

#include "mesh_deformator.hpp"

#include <qce/qce.hpp>
#include <qce/ancillary/directories.hpp>

int main(int argc, char* argv[]) {
#ifdef NDEBUG
    FreeConsole();
#endif

    auto& app = QCE::Application<
        HillsAnimationSystem
    >::Get();
    QCE_CRITICAL(
    app.Setup<
        HillsAnimationConfig
    >());

    QCE_CRITICAL(app.LoadScene("game_demo"));

    QCE_CRITICAL(
        app.m_systems.Get<HillsAnimationSystem>().UpdateScene());
    return app.Run();
}