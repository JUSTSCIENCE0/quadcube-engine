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

#endif