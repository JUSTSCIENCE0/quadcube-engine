// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

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

using QCEResourceType = QCE::ResourceType;
MJSON_ENUM_BEGIN(QCEResourceType,
        "Resource Type",
        "Type of resource used in the scene")
    MJSON_ENUM_UNIT(QCE::E_RESTYPE_FIGURE,    figure)
    MJSON_ENUM_UNIT(QCE::E_RESTYPE_MESH,      mesh)
    MJSON_ENUM_UNIT(QCE::E_RESTYPE_TEXTURE,   texture)
    MJSON_ENUM_UNIT(QCE::E_RESTYPE_MATERIAL,  material)
    MJSON_ENUM_UNIT(QCE::E_RESTYPE_ANIMATION, animation)
MJSON_ENUM_END(QCEResourceType)

using QCEFigureType = QCE::FigureType;
MJSON_ENUM_BEGIN(QCEFigureType,
        "Figure Type",
        "Type of figure used as mesh in the scene")
    MJSON_ENUM_UNIT(QCE::E_FIGTYPE_CUBOID, cuboid)
    MJSON_ENUM_UNIT(QCE::E_FIGTYPE_SPHERE, sphere)
    MJSON_ENUM_UNIT(QCE::E_FIGTYPE_PLANE,  plane)
MJSON_ENUM_END(QCEFigureType)

#endif