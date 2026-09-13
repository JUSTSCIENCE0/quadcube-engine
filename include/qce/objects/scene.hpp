// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/resource_manager.hpp>

#include <vector>
#include <string>
#include <variant>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM_TYPED(ResourceType, int8_t) \
        CU_ENUM_UNIT(E_RESTYPE_FIGURE) \
        CU_ENUM_UNIT(E_RESTYPE_MESH) \
        CU_ENUM_UNIT(E_RESTYPE_TEXTURE) \
        CU_ENUM_UNIT(E_RESTYPE_MATERIAL) \
        CU_ENUM_UNIT(E_RESTYPE_ANIMATION) \
        CU_ENUM_ANCILLARY_UNITS(E_RESTYPE) \
    CU_END_ENUM(ResourceType) \
    CU_BEGIN_ENUM_TYPED(FigureType, int8_t) \
        CU_ENUM_UNIT(E_FIGTYPE_CUBOID) \
        CU_ENUM_UNIT(E_FIGTYPE_SPHERE) \
        CU_ENUM_UNIT(E_FIGTYPE_PLANE) \
        CU_ENUM_ANCILLARY_UNITS(E_FIGTYPE) \
    CU_END_ENUM(FigureType)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    using ResourceParams = std::variant<
        CuboidParams,
        SphereParams,
        PlaneParams,
        MeshParams,
        MaterialParams
    >;

    struct ResourceDescription {
        ResourceType   type;
        std::string    name;
        ResourceParams params;
    };

    struct SceneDescription {
        std::vector<ResourceDescription> resources;
    };
}
