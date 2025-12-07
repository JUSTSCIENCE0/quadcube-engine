// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

cbuffer cbPerObject : register(b0) {
    float4x4 gWorldViewProj;
};

struct VertexIn {
    float3 PosL : POSITION;
};

struct VertexOut {
    float4 PosH : SV_POSITION;
};

VertexOut main(VertexIn vin) {
    VertexOut vout;

    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

    return vout;
}
