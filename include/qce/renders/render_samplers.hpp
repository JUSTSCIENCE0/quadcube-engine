// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// Default samplers. You can redefine this macro to provide your own samplers.
// Sampler description types:
// QCE_PIXEL_SAMPLER(address_mode, minLOD, maxLOD)
// QCE_BILINEAR_SAMPLER(address_mode, minLOD, maxLOD)
// QCE_TRILINEAR_SAMPLER(address_mode, minLOD, maxLOD)
// QCE_ANISOTROPIC_SAMPLER(address_mode, minLOD, maxLOD, anisotropy_level)
//
// Address modes:
// - Wrap
// - Border

#pragma once
#ifdef QCE_RENDER_SAMPLERS_HEADER
#  include QCE_RENDER_SAMPLERS_HEADER
#else

#  define QCE_RENDER_SAMPLERS \
    QCE_BILINEAR_SAMPLER(Wrap, 0, MAX_LOD) \
    QCE_ANISOTROPIC_SAMPLER(Wrap, 0, MAX_LOD, 16)

#endif

#include <stdint.h>
#include <limits>

namespace QCE {
    static constexpr auto MAX_LOD = std::numeric_limits<size_t>::max();

    namespace SamplerAddressMode {
        struct Wrap {};
        struct Border {};

        // TODO: Add more address modes if needed
    }
}
