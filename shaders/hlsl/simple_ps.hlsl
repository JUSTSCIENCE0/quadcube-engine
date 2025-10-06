// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

cbuffer cbPerObject : register(b0) {
    float4x4 gWorldViewProj;
};

struct VertexOut {
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(VertexOut pin) : SV_Target {
    return pin.Color;
}

