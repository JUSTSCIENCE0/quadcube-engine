// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef MJSON_OBJECTS_GENERATION_PART

using QCEEasingFunc = QCE::EasingFunc;
MJSON_ENUM_BEGIN(QCEEasingFunc,
        "Easing Function", "Type of easing function used in animation segment")
    MJSON_ENUM_UNIT(QCE::E_EASING_LINEAR,        linear)
    MJSON_ENUM_UNIT(QCE::E_EASING_EASE_IN_QUAD,  easy_in_quad)
    MJSON_ENUM_UNIT(QCE::E_EASING_EASE_IN_EXPO,  easy_in_expo)
    MJSON_ENUM_UNIT(QCE::E_EASING_EASE_OUT_QUAD, easy_out_quad)
    MJSON_ENUM_UNIT(QCE::E_EASING_EASE_OUT_SQRT, easy_out_sqrt)
    MJSON_ENUM_UNIT(QCE::E_EASING_SMOOTH_STEP,   smooth_step)
    MJSON_ENUM_UNIT(QCE::E_EASING_SMOOTHER_STEP, smoother_step)
MJSON_ENUM_END(QCEEasingFunc)

using QCESplineFunc = QCE::SplineFunc;
MJSON_ENUM_BEGIN(QCESplineFunc,
        "Spline Function", "Spline function used for position interpolation between key frames")
    MJSON_ENUM_UNIT(QCE::E_SPLINE_LINEAR,      linear)
    MJSON_ENUM_UNIT(QCE::E_SPLINE_CATMULL_ROM, catmull_rom)
MJSON_ENUM_END(QCESplineFunc)

MJSON_OBJECT_BEGIN(QCE::AnimationRotationKey,
        "Animation Rotation Key",
        "Key frame describing rotation channel animation")
    MJSON_FIELD(QCE::quaternion, value,      "Rotation", nullptr)
    MJSON_FIELD(float,           start_time, "Start Time", nullptr)
    MJSON_FIELD(QCE::EasingFunc, easing,     "Easing Function", nullptr)
MJSON_OBJECT_END(QCE::AnimationRotationKey)

MJSON_OBJECT_BEGIN(QCE::AnimationPositionKey,
        "Animation Position Key",
        "Key frame describing position channel animation")
    MJSON_FIELD(QCE::float3d,    value,      "Position", nullptr)
    MJSON_FIELD(float,           start_time, "Start Time", nullptr)
    MJSON_FIELD(QCE::EasingFunc, easing,     "Easing Function", nullptr)
MJSON_OBJECT_END(QCE::AnimationPositionKey)

MJSON_OBJECT_BEGIN(QCE::AnimationScaleKey,
        "Animation Scale Key",
        "Key frame describing scale channel animation")
    MJSON_FIELD(QCE::float3d,    value,      "Scale", nullptr)
    MJSON_FIELD(float,           start_time, "Start Time", nullptr)
    MJSON_FIELD(QCE::EasingFunc, easing,     "Easing Function", nullptr)
MJSON_OBJECT_END(QCE::AnimationScaleKey)

MJSON_OBJECT_BEGIN(QCE::TransformAnimation,
        "Transform Animation",
        "Animation of a transform component")
    MJSON_FIELD(std::string, id, "ID", nullptr)
    MJSON_FIELD(std::vector<QCE::AnimationRotationKey>, rotation_channel, "Rotation channel", nullptr)
    MJSON_FIELD(std::vector<QCE::AnimationPositionKey>, position_channel, "Position channel", nullptr)
    MJSON_FIELD(std::vector<QCE::AnimationScaleKey>,    scale_channel,    "Scale channel", nullptr)
    MJSON_FIELD(QCE::SplineFunc, spline_func, "Spline Function", nullptr)
MJSON_OBJECT_END(QCE::TransformAnimation)

#endif
