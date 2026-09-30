// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// the engine objects have already been declared and defined within the engine
// do not generate here
#ifndef MJSON_OBJECTS_GENERATION_PART

MJSON_OBJECT_BEGIN(QCE::CuboidParams, "Cuboid parameters", "Parameters for cuboid mesh generation")
    MJSON_FIELD(float, length, "Length", nullptr)
    MJSON_FIELD(float, width,  "Width",  nullptr)
    MJSON_FIELD(float, height, "Height", nullptr)
MJSON_OBJECT_END(QCE::CuboidParams)

MJSON_OBJECT_BEGIN(QCE::SphereParams, "Sphere parameters", "Parameters for sphere mesh generation")
    MJSON_FIELD(float, radius,       "Radius", nullptr)
    MJSON_FIELD(int,   subdivisions, "Subdivisions", nullptr)
    MJSON_FIELD(bool,  hard_edges,   "Hard Edges", nullptr)
MJSON_OBJECT_END(QCE::SphereParams)

MJSON_OBJECT_BEGIN(QCE::PlaneParams, "Plane parameters", "Parameters for plane mesh generation")
    MJSON_FIELD(float, length,       "Length", nullptr)
    MJSON_FIELD(float, width,        "Width",  nullptr)
    MJSON_FIELD(bool,  hard_edges,   "Hard Edges", nullptr)
    MJSON_FIELD(bool,  repeat_uv,    "Repeat UV", nullptr)
    MJSON_FIELD(bool,  unit_squares, "Unit Squares", nullptr)
MJSON_OBJECT_END(QCE::PlaneParams)

MJSON_OBJECT_BEGIN(QCE::MeshParams, "Mesh parameters", "Describe additional mesh params")
    MJSON_FIELD(bool, is_empty, "Is Empty", nullptr)
MJSON_OBJECT_END(QCE::MeshParams)

using QCETextureType = QCE::TextureType;
MJSON_ENUM_BEGIN(QCETextureType, "Texture type", nullptr)
    MJSON_ENUM_UNIT(QCE::E_TEXTYPE_2D,       texture_2d)
    MJSON_ENUM_UNIT(QCE::E_TEXTYPE_CUBE_MAP, cube_map)
MJSON_ENUM_END(QCETextureType)

MJSON_OBJECT_BEGIN(QCE::TextureParams, "Texture parameters", "Describe additional texture params")
    MJSON_FIELD(QCE::TextureType, texture_type, "Texture type", nullptr)
MJSON_OBJECT_END(QCE::TextureParams)

MJSON_OBJECT_BEGIN(QCE::MaterialParams, "Materal parameters", "Describe material")
    MJSON_FIELD(QCE::color_rgba, albedo_color, "Albedo color", nullptr)
    MJSON_FIELD(QCE::float3d, fresnel, "Fresnel", nullptr)
    MJSON_FIELD(float, shininess, "Shininess", nullptr)
    MJSON_FIELD(std::optional<std::string>, albedo_texture,    "Albedo texture", nullptr)
    MJSON_FIELD(std::optional<std::string>, normal_texture,    "Normal texture", nullptr)
    MJSON_FIELD(std::optional<std::string>, metallic_texture,  "Metallic texture", nullptr)
    MJSON_FIELD(std::optional<std::string>, roughness_texture, "Roughness texture", nullptr)
    MJSON_FIELD(std::optional<std::string>, occlusion_texture, "Occlusion texture", nullptr)
    MJSON_FIELD(std::optional<std::string>, emissive_texture,  "Emissive texture", nullptr)
MJSON_OBJECT_END(QCE::MaterialParams)

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

using QCEAnimationType = QCE::AnimationType;
MJSON_ENUM_BEGIN(QCEAnimationType, "Animation type", nullptr)
    MJSON_ENUM_UNIT(QCE::E_ANIMATION_TRANSFORM, transform_animation)
MJSON_ENUM_END(QCEAnimationType)

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
    MJSON_FIELD(float, total_duration, "Total Duration", nullptr)
    MJSON_FIELD(QCE::SplineFunc, spline_func, "Spline Function", nullptr)
MJSON_OBJECT_END(QCE::TransformAnimation)

MJSON_OBJECT_BEGIN(QCE::AnimationParams, "Animation parameters", "Describe additional animation params")
    MJSON_FIELD(QCE::AnimationType, animation_type, "Animation type", nullptr)
MJSON_OBJECT_END(QCE::AnimationParams)

#endif
