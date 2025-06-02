// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

//#include <qce/math.hpp>
#include <qce/math/vector_sse2.hpp>
#include <array>

using namespace QCE;

int main() {
    auto v1 = vector_init(1.0f, 2.0f, 3.0f, 4.0f);
    auto v2 = vector_init(4, 3, 2, 1);

    auto vp = v1 + v2;
    auto vs = v1 - v2;
    auto vm = v1 * v2;
    auto vd = v1 / v2;

    auto vms = v1 * 2.0;
    auto vds = v1 / 2.0;

    return 0;
}