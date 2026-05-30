// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include "common.hlsl"

float4 main(VertexOut pin) : SV_Target {
    pin.normal_w = normalize(pin.normal_w);

    /*
    float3 to_eye_direction = normalize(eye_position - pin.pos_w);
    float3 ambient = ambient_light_color * albedo_color;
    Material material = { albedo_color, fresnel, shininess };
    float4 direct_light = lighting(
        scene_light, material, pin.pos_w,
        pin.normal_w, to_eye_direction);

    float4 color = ambient + direct_light;
    color.a = albedo_color.a;

    return color;
    */
    return albedo_color;

}

