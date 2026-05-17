// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include "common.hlsl"

VertexOut main(VertexIn vin) {
    VertexOut vout;

    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
    vout.Color = vin.Color;

    return vout;
}
