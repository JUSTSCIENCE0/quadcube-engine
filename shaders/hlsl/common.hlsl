// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include "base_lighting.hlsl"

Texture2D diffuse_map : register(t0);

cbuffer UnitConstants : register(b0) {
    float4x4 world_matrix;
};

cbuffer MaterialConstants : register(b1) {
    float4 albedo_color;
    float3 fresnel;
    float  shininess;
};

cbuffer PassConstants : register(b2) {
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

    float4      ambient_light_color;
    SceneLights scene_lights;
};

struct VertexIn {
    float3 pos_l : POSITION;
    float3 normal_l : NORMAL;
};

struct VertexOut {
    float4 pos_h : SV_POSITION;
    float3 pos_w : POSITION;
    float3 normal_w : NORMAL;
};

