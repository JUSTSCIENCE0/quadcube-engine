// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/figures.hpp>

struct DeformationDescription {
    float update_period_sec;
    QCE::PlaneParams plane_params;
    bool is_reflected;
};

#define QCE_ECS_ENTITY_MANAGER_CACHE_SIZE 64
#define QCE_ECS_ADDITIONAL_COMPONENTS DeformationDescription