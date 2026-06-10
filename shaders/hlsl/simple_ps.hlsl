// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include "common.hlsl"

float4 main(VertexOut pin) : SV_Target {
    float4 albedo = 
        diffuse_map.Sample(bilinear_sampler, pin.uv_coord) * 
        albedo_color;

    pin.normal_w = normalize(pin.normal_w);
    float3 to_eye_direction = normalize(eye_position - pin.pos_w);
    float4 ambient = ambient_light_color * albedo;
    Material material = { albedo, fresnel, shininess };
    float4 direct_light = lighting(
        scene_lights, material, pin.pos_w,
        pin.normal_w, to_eye_direction);

    float4 color = ambient + direct_light;
    color.a = albedo.a;

    return color;

}

