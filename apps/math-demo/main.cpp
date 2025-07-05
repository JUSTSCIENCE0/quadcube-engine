// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

//#include <qce/math.hpp>
#include <qce/math/vector_avx512.hpp>

using namespace QCE;

int main() {
    float mem[] = { 4.0f, 3.0f, 2.0f, 1.0f };

    auto v1 = vector_init(1.0f, 2.0f, 3.0f, 4.0f);
    auto v2 = vector_init(mem);

    auto vp = v1 + v2;
    auto vs = v1 - v2;
    auto vm = v1 * v2;
    auto vd = v1 / v2;

    auto vms = v1 * 2.0;
    auto vds = v1 / 2.0;

    auto crossv = vector_cross_product(v1, v2);

    auto len = vector_length(v1);
    (void)len;

    vector_copy(vp, mem);

    return 0;
}