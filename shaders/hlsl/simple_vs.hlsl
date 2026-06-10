// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include "common.hlsl"

VertexOut main(VertexIn vin) {
    VertexOut vout;

    float4 pos_w = mul(float4(vin.pos_l, 1.0f), world_matrix);
    vout.pos_w = pos_w.xyz;
    vout.pos_h = mul(pos_w, view_proj_matrix);

    vout.normal_w = mul(vin.normal_l, (float3x3) world_matrix);

    vout.uv_coord = vin.uv_coord;

    return vout;
}
