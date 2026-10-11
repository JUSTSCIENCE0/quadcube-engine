// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef MJSON_OBJECTS_GENERATION_PART

MJSON_OBJECT_BEGIN(DeformationDescription, "Deformation Description", nullptr)
    MJSON_FIELD(float, update_period_sec, "Update Period", nullptr)
    MJSON_FIELD(QCE::PlaneParams, plane_params, "Plane Parameters", nullptr)
    MJSON_FIELD(bool, is_reflected, "Is Reflected", nullptr)
MJSON_OBJECT_END(DeformationDescription)

#endif
