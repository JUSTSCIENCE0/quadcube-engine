// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/renders/render_samplers.hpp>

#include <directx/d3dx12.h>

#include <array>
#include <utility>

namespace QCE {
    namespace PrivateImplementation {
        template<typename SamplerModeT>
        constexpr D3D12_TEXTURE_ADDRESS_MODE dx12_get_address_mode() {
            if constexpr (std::same_as<SamplerModeT, SamplerMode::REPEAT>) {
                return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            }
            else if constexpr (std::same_as<SamplerModeT, SamplerMode::BLACK>) {
                return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
            }
            else {
                static_assert(
                    sizeof(SamplerModeT) == 0 /* false */,
                    "Unsupported sampler mode");
            }
        }

        template<typename SamplerModeT>
        constexpr D3D12_STATIC_BORDER_COLOR dx12_get_border_color() {
            if constexpr (std::same_as<SamplerModeT, SamplerMode::REPEAT>) {
                return D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            }
            else if constexpr (std::same_as<SamplerModeT, SamplerMode::BLACK>) {
                return D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
            }
            else {
                static_assert(
                    sizeof(SamplerModeT) == 0 /* false */,
                    "Unsupported sampler mode");
            }
        }

        constexpr FLOAT dx12_decode_sampler_lod(size_t lod) {
            if (lod == MAX_LOD)
                return D3D12_FLOAT32_MAX;
            else
                return static_cast<FLOAT>(lod);
        }
    }

    static inline std::array<const CD3DX12_STATIC_SAMPLER_DESC, E_SAMPLER_COUNT> dx12_get_static_samplers() {
#define QCE_DX12_STATIC_SAMPLER(index, filter, address_mode, minLOD, maxLOD, anisotropy_level) \
    CD3DX12_STATIC_SAMPLER_DESC( \
        /*shaderRegister*/ index, \
        /*filter*/         filter, \
        /*addressU*/       PrivateImplementation::dx12_get_address_mode<SamplerMode::address_mode>(), \
        /*addressV*/       PrivateImplementation::dx12_get_address_mode<SamplerMode::address_mode>(), \
        /*addressW*/       PrivateImplementation::dx12_get_address_mode<SamplerMode::address_mode>(), \
        /*mipLODBias*/     0.0f, \
        /*maxAnisotropy*/  anisotropy_level, \
        /*comparisonFunc*/ D3D12_COMPARISON_FUNC_LESS_EQUAL, \
        /*borderColor*/    PrivateImplementation::dx12_get_border_color<SamplerMode::address_mode>(), \
        /*minLOD*/         minLOD, \
        /*maxLOD*/         PrivateImplementation::dx12_decode_sampler_lod(maxLOD) \
    ),
#define QCE_PIXEL_SAMPLER(name_suffix, address_mode, minLOD, maxLOD) \
    QCE_DX12_STATIC_SAMPLER( \
        /*index*/    E_SAMPLER_PIXEL_ ##address_mode ##name_suffix , \
        /*filter*/   D3D12_FILTER_MIN_MAG_MIP_POINT, \
        address_mode, minLOD, maxLOD, 0 )
#define QCE_BILINEAR_SAMPLER(name_suffix, address_mode, minLOD, maxLOD) \
    QCE_DX12_STATIC_SAMPLER( \
        /*index*/    E_SAMPLER_BILINEAR_ ##address_mode ##name_suffix , \
        /*filter*/   D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT, \
        address_mode, minLOD, maxLOD, 0 )
#define QCE_TRILINEAR_SAMPLER(name_suffix, address_mode, minLOD, maxLOD) \
    QCE_DX12_STATIC_SAMPLER( \
        /*index*/    E_SAMPLER_TRILINEAR_ ##address_mode ##name_suffix , \
        /*filter*/   D3D12_FILTER_MIN_MAG_MIP_LINEAR, \
        address_mode, minLOD, maxLOD, 0 )
#define QCE_ANISOTROPIC_SAMPLER(name_suffix, address_mode, minLOD, maxLOD, anisotropy_level) \
    QCE_DX12_STATIC_SAMPLER( \
        /*index*/    E_SAMPLER_ANISOTROPIC ##anisotropy_level ##_ ##address_mode ##name_suffix , \
        /*filter*/   D3D12_FILTER_ANISOTROPIC, \
        address_mode, minLOD, maxLOD, anisotropy_level )

        std::array<const CD3DX12_STATIC_SAMPLER_DESC, E_SAMPLER_COUNT> samplers{
            QCE_RENDER_SAMPLERS
        };

#undef QCE_PIXEL_SAMPLER
#undef QCE_BILINEAR_SAMPLER
#undef QCE_TRILINEAR_SAMPLER
#undef QCE_ANISOTROPIC_SAMPLER
#undef QCE_DX12_STATIC_SAMPLER

        return samplers;
    }
}