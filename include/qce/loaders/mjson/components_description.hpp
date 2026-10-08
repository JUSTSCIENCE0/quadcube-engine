// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// the engine objects have already been declared and defined within the engine
// do not generate here
#ifndef MJSON_OBJECTS_GENERATION_PART

MJSON_OBJECT_BEGIN(QCE::DirectionalLight, "Directional Light", "Directional light component")
    MJSON_FIELD(QCE::color_rgba, color, "Color", nullptr)
    MJSON_FIELD(QCE::float3d, direction, "Direction", nullptr)
MJSON_OBJECT_END(QCE::DirectionalLight)

MJSON_OBJECT_BEGIN(QCE::PointLight, "Point Light", "Point light component")
    MJSON_FIELD(QCE::color_rgba, color, "Color", nullptr)
    MJSON_FIELD(QCE::float3d, position, "Position", nullptr)
    MJSON_FIELD(float, falloff_begin, "Falloff Begin", nullptr)
    MJSON_FIELD(float, falloff_end, "Falloff End", nullptr)
MJSON_OBJECT_END(QCE::PointLight)

MJSON_OBJECT_BEGIN(QCE::SpotLight, "Spot Light", "Spot light component")
    MJSON_FIELD(QCE::color_rgba, color, "Color", nullptr)
    MJSON_FIELD(QCE::float3d, position, "Position", nullptr)
    MJSON_FIELD(QCE::float3d, direction, "Direction", nullptr)
    MJSON_FIELD(float, falloff_begin, "Falloff Begin", nullptr)
    MJSON_FIELD(float, falloff_end, "Falloff End", nullptr)
    MJSON_FIELD(float, spot_power, "Spot Power", nullptr)
MJSON_OBJECT_END(QCE::SpotLight)

MJSON_OBJECT_BEGIN(QCE::TransformComponents, "Transform Components", "Entity transform component")
    MJSON_FIELD(QCE::quaternion, rotation, "Rotation", nullptr)
    MJSON_FIELD(QCE::float3d, position, "Position", nullptr)
    MJSON_FIELD(QCE::float3d, scale, "Scale", nullptr)
MJSON_OBJECT_END(QCE::TransformComponents)

MJSON_OBJECT_BEGIN(QCE::TransformAnimationDescription, "Transform Animation Description", "Entity transform animation settings")
    MJSON_FIELD(std::string, animation_name, "Animation Name", nullptr)
    MJSON_FIELD(bool, is_looped, "Is Looped", nullptr)
MJSON_OBJECT_END(QCE::TransformAnimationDescription)

#endif