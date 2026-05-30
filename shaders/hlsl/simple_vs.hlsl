// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include "common.hlsl"

VertexOut main(VertexIn vin) {
    VertexOut vout;

    float4 pos_w = mul(float4(vin.pos_l, 1.0f), world_matrix);
    vout.pos_h = mul(pos_w, view_proj_matrix);

    return vout;
}
