// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

cbuffer UnitConstants : register(b0)
{
    float4x4 world_matrix;
};

cbuffer MaterialConstants : register(b1)
{
    float4 albedo_color;
    float3 fresnel;
    float  roughness;
};

cbuffer PassConstants : register(b2)
{
    float4x4 view_matrix;
    float4x4 view_matrix_inv;
    float4x4 proj_matrix;
    float4x4 inv_proj_matrix;
    float4x4 view_proj_matrix;
    float4x4 inv_view_proj_matrix;
    float3   eye_position;
    float    padding1;
    float2   render_target_size;
    float2   render_target_size_inv;
    float    near_z;
    float    far_z;
    float    delta_time;
    float    padding2;
};

struct VertexIn
{
    float3 pos_l : POSITION;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 pos_h : SV_POSITION;
    float4 color : COLOR;
};

