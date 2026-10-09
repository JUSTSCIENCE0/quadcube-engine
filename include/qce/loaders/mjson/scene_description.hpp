// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// ancillary enum only for json read/write
// generate object declaration and definision here
MJSON_ENUM_BEGIN(ResourceType,
        "Resource Type", "Type of resource in scene")
    MJSON_ENUM_UNIT(E_SCENE_RESOURCE_CUBOID,    cuboid)
    MJSON_ENUM_UNIT(E_SCENE_RESOURCE_SPHERE,    sphere)
    MJSON_ENUM_UNIT(E_SCENE_RESOURCE_PLANE,     plane)
    MJSON_ENUM_UNIT(E_SCENE_RESOURCE_MESH,      mesh)
    MJSON_ENUM_UNIT(E_SCENE_RESOURCE_TEXTURE,   texture)
    MJSON_ENUM_UNIT(E_SCENE_RESOURCE_MATERIAL,  material)
    MJSON_ENUM_UNIT(E_SCENE_RESOURCE_ANIMATION, animation)
MJSON_ENUM_END(ResourceType)

// the engine objects have already been declared and defined within the engine
// do not generate here
#ifndef MJSON_OBJECTS_GENERATION_PART

MJSON_VARIANT_BEGIN(QCE::ResourceParams, ResourceType,
        "Resource parameters", "")
    MJSON_VARIANT_UNIT(QCE::CuboidParams,    E_SCENE_RESOURCE_CUBOID)
    MJSON_VARIANT_UNIT(QCE::SphereParams,    E_SCENE_RESOURCE_SPHERE)
    MJSON_VARIANT_UNIT(QCE::PlaneParams,     E_SCENE_RESOURCE_PLANE)
    MJSON_VARIANT_UNIT(QCE::MeshParams,      E_SCENE_RESOURCE_MESH)
    MJSON_VARIANT_UNIT(QCE::TextureParams,   E_SCENE_RESOURCE_TEXTURE)
    MJSON_VARIANT_UNIT(QCE::MaterialParams,  E_SCENE_RESOURCE_MATERIAL)
    MJSON_VARIANT_UNIT(QCE::AnimationParams, E_SCENE_RESOURCE_ANIMATION)
MJSON_VARIANT_END(QCE::ResourceParams)

MJSON_OBJECT_BEGIN(QCE::ResourceDescription, "Resource description", nullptr)
    MJSON_FIELD(std::string, name, "Name", nullptr)
    MJSON_FIELD(QCE::ResourceParams, params, "Parameters", nullptr)
MJSON_OBJECT_END(QCE::ResourceDescription)

MJSON_OBJECT_BEGIN(QCE::EntityDescription, "Entity description", nullptr)
    MJSON_FIELD(std::optional<std::string>, name, "Name", nullptr)
    MJSON_FIELD(std::optional<std::string>, static_mesh, "Static Mesh", nullptr)
    MJSON_FIELD(std::optional<QCE::DynamicMeshDescription>, dynamic_mesh, "Dynamic Mesh", nullptr)
    MJSON_FIELD(std::optional<std::string>, material, "Material", nullptr)
    MJSON_FIELD(std::optional<QCE::DirectionalLight>, directional_light, "Directional Light", nullptr)
    MJSON_FIELD(std::optional<QCE::PointLight>, point_light, "Point Light", nullptr)
    MJSON_FIELD(std::optional<QCE::SpotLight>, spot_light, "Spot Light", nullptr)
    MJSON_FIELD(std::optional<QCE::TransformComponents>, transform, "Transform", nullptr)
    MJSON_FIELD(std::optional<QCE::TransformAnimationDescription>, transform_animation, "Transform Animation", nullptr)
MJSON_OBJECT_END(QCE::EntityDescription)

MJSON_OBJECT_BEGIN(QCE::SceneDescription, "Scene description", nullptr)
    MJSON_FIELD(std::vector<QCE::ResourceDescription>, resources, "Resources", nullptr)
    MJSON_FIELD(std::vector<QCE::EntityDescription>,   entities,  "Entities", nullptr)
MJSON_OBJECT_END(QCE::SceneDescription)

#endif