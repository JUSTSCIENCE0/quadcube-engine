// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>
#include <qce/ancillary/error_codes.hpp>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM_TYPED(FloatQuantization, int8_t) \
        CU_VALUED_ENUM_UNIT(E_0_BIT_FQ,  0) \
        CU_VALUED_ENUM_UNIT(E_8_BIT_FQ,  1) \
        CU_VALUED_ENUM_UNIT(E_16_BIT_FQ, 2) \
        CU_VALUED_ENUM_UNIT(E_24_BIT_FQ, 3) \
        CU_VALUED_ENUM_UNIT(E_NO_FQ,     4) \
    CU_END_ENUM(FloatQuantization) \
    CU_BEGIN_ENUM_TYPED(QuaternionQuantization, int8_t) \
        CU_VALUED_ENUM_UNIT(E_32_BIT_QQ, 0) \
        CU_VALUED_ENUM_UNIT(E_64_BIT_QQ, 1) \
        CU_VALUED_ENUM_UNIT(E_NO_QQ,     3) \
    CU_END_ENUM(QuaternionQuantization)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    struct TransformAnimation;

    struct TransformAnimationCompressionParams {
        float position_eps = 0.0005f;
        float scale_eps = 0.0005f;
        QuaternionQuantization rotation_quantization = E_32_BIT_QQ;
    };

    struct TransformAnimationCompressionContext {
        bool has_position_channel = false;
        FloatQuantization position_quantization[3] = {
            E_NO_FQ, E_NO_FQ, E_NO_FQ };
        float3d position_min{};
        float3d position_max{};

        bool has_scale_channel = false;
        FloatQuantization scale_quantization[3] = {
            E_NO_FQ, E_NO_FQ, E_NO_FQ };
        float3d scale_min{};
        float3d scale_max{};

        bool has_rotation_channel = false;
        QuaternionQuantization rotation_quantization = E_32_BIT_QQ;
    };

    TransformAnimationCompressionContext calculate_compression_context(
        const TransformAnimation& animation,
        float position_eps = 0.0005f,
        float scale_eps = 0.0005f,
        QuaternionQuantization qq = E_32_BIT_QQ);
}
