// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef MJSON_OBJECTS_GENERATION_PART

MJSON_ENUM_BEGIN(EasingFunc,
        "Easing Function", "Type of easing function used in animation segment")
    MJSON_ENUM_UNIT(E_EASING_LINEAR,        linear)
    MJSON_ENUM_UNIT(E_EASING_EASE_IN_QUAD,  easy_in_quad)
    MJSON_ENUM_UNIT(E_EASING_EASE_IN_EXPO,  easy_in_expo)
    MJSON_ENUM_UNIT(E_EASING_EASE_OUT_QUAD, easy_out_quad)
    MJSON_ENUM_UNIT(E_EASING_EASE_OUT_SQRT, easy_out_sqrt)
    MJSON_ENUM_UNIT(E_EASING_SMOOTH_STEP,   smooth_step)
    MJSON_ENUM_UNIT(E_EASING_SMOOTHER_STEP, smoother_step)
MJSON_ENUM_END(EasingFunc)

MJSON_OBJECT_BEGIN(
        AnimationPositionKey,
        "Animation Position Key",
        "Key frame describing position channel animation")
    MJSON_FIELD(QCE::float3d, value,      "Position", nullptr)
    MJSON_FIELD(float,        start_time, "Start Time", nullptr)
    MJSON_FIELD(EasingFunc,   easing,     "Easing Function", nullptr)
MJSON_OBJECT_END(AnimationPositionKey)

#endif
