// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// Default samplers. You can redefine this macro to provide your own samplers.
// Sampler description types:
// QCE_PIXEL_SAMPLER(name_suffix, address_mode, minLOD, maxLOD)
// QCE_BILINEAR_SAMPLER(name_suffix, address_mode, minLOD, maxLOD)
// QCE_TRILINEAR_SAMPLER(name_suffix, address_mode, minLOD, maxLOD)
// QCE_ANISOTROPIC_SAMPLER(name_suffix, address_mode, minLOD, maxLOD, anisotropy_level)
//
// Address modes:
// - REPEAT
// - BLACK

#pragma once
#ifdef QCE_RENDER_SAMPLERS_HEADER
#  include QCE_RENDER_SAMPLERS_HEADER
#else

#  define QCE_RENDER_SAMPLERS \
    QCE_BILINEAR_SAMPLER(/*no suffix*/, REPEAT, 0, MAX_LOD) \
    QCE_ANISOTROPIC_SAMPLER(/*no suffix*/, REPEAT, 0, MAX_LOD, 16)

#endif

#include <stdint.h>
#include <limits>

namespace QCE {
    static constexpr auto MAX_LOD = std::numeric_limits<size_t>::max();

    namespace SamplerMode {
        struct REPEAT {};
        struct BLACK {};

        // TODO: Add more address modes if needed
    }

// create samplers enum
#define QCE_PIXEL_SAMPLER(name_suffix, address_mode, minLOD, maxLOD) \
    CU_ENUM_UNIT(E_SAMPLER_PIXEL_ ##address_mode ##name_suffix)
#define QCE_BILINEAR_SAMPLER(name_suffix, address_mode, minLOD, maxLOD) \
    CU_ENUM_UNIT(E_SAMPLER_BILINEAR_ ##address_mode ##name_suffix)
#define QCE_TRILINEAR_SAMPLER(name_suffix, address_mode, minLOD, maxLOD) \
    CU_ENUM_UNIT(E_SAMPLER_TRILINEAR_ ##address_mode ##name_suffix)
#define QCE_ANISOTROPIC_SAMPLER(name_suffix, address_mode, minLOD, maxLOD, anisotropy_level) \
    CU_ENUM_UNIT(E_SAMPLER_ANISOTROPIC ##anisotropy_level ##_ ##address_mode ##name_suffix)

#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(Sampler) \
        QCE_RENDER_SAMPLERS \
        CU_ENUM_ANCILLARY_UNITS(E_SAMPLER) \
    CU_END_ENUM(Sampler)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

#undef QCE_PIXEL_SAMPLER
#undef QCE_BILINEAR_SAMPLER
#undef QCE_TRILINEAR_SAMPLER
#undef QCE_ANISOTROPIC_SAMPLER
}
