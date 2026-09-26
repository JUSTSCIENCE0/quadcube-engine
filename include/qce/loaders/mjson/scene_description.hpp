// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef MJSON_OBJECTS_GENERATION_PART

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