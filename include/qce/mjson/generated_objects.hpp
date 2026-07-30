// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/mjson/math_types.hpp>
#include <qce/objects/animation.hpp>

// TODO: try to use namespaces in macros and remove this
using EasingFunc = QCE::EasingFunc;
static constexpr auto E_EASING_LINEAR        = QCE::EasingFunc::E_EASING_LINEAR;
static constexpr auto E_EASING_EASE_IN_QUAD  = QCE::EasingFunc::E_EASING_EASE_IN_QUAD;
static constexpr auto E_EASING_EASE_IN_EXPO  = QCE::EasingFunc::E_EASING_EASE_IN_EXPO;
static constexpr auto E_EASING_EASE_OUT_QUAD = QCE::EasingFunc::E_EASING_EASE_OUT_QUAD;
static constexpr auto E_EASING_EASE_OUT_SQRT = QCE::EasingFunc::E_EASING_EASE_OUT_SQRT;
static constexpr auto E_EASING_SMOOTH_STEP   = QCE::EasingFunc::E_EASING_SMOOTH_STEP;
static constexpr auto E_EASING_SMOOTHER_STEP = QCE::EasingFunc::E_EASING_SMOOTHER_STEP;
using AnimationRotationKey = QCE::AnimationRotationKey;
using AnimationPositionKey = QCE::AnimationPositionKey;
using AnimationScaleKey    = QCE::AnimationScaleKey;

#include "mjson_output.hpp"