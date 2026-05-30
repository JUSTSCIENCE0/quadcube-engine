// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#define MAX_LIGHTS 16

struct Light {
    float3 color;
    float  falloff_begin;
    float3 direction;
    float  falloff_end;
    float3 position;
    float  spot_power;
};

struct SceneLights
{
    Light lights[MAX_LIGHTS];

    int directional_light_end;
    int point_light_end;
    int spot_light_end;
    int padding; // for alignment
};

struct Material {
    float4 albedo_color;
    float3 fresnel;
    float  shininess;
};

/// helpers

float attenuation(float d, float falloff_begin, float falloff_end) {
    // TODO: phisically correct attenuation
    return saturate((falloff_end - d) / (falloff_end - falloff_begin));
}

// Schlick's approximation to Fresnel reflectance
// R0 = ( (n-1)/(n+1) )^2, where n is the index of refraction.
float3 schlick_fresnel(float3 r0, float3 normal, float3 light_vector) {
    float cos_incident_angle = saturate(dot(normal, light_vector));

    float f0 = 1.0f - cos_incident_angle;
    float3 reflect_percent = r0 + (1.0f - r0) * (f0 * f0 * f0 * f0 * f0);

    return reflect_percent;
}

float3 blinn_phong_model(
        float3 light_strength, float3 light_vector, float3 normal, float3 to_eye_direction, Material material) {
    const float m = material.shininess * 256.0f;
    float3 half_vector = normalize(to_eye_direction + light_vector);

    float roughness_factor = (m + 8.0f) * pow(max(dot(half_vector, normal), 0.0f), m) / 8.0f;
    float3 fresnel_factor = schlick_fresnel(material.fresnel, half_vector, light_vector);

    float3 spec_albedo = fresnel_factor * roughness_factor;

    // TODO: HDR
    spec_albedo = spec_albedo / (spec_albedo + 1.0f);

    return (material.albedo_color.rgb + spec_albedo) * light_strength;
}

float3 directional_light(Light light, Material material, float3 normal, float3 to_eye_direction) {
    float3 light_vector = -light.direction;

    float ndotl = max(dot(light_vector, normal), 0.0f);
    float3 light_strength = light.color * ndotl;

    return blinn_phong_model(light_strength, light_vector, normal, to_eye_direction, material);
}

float3 point_light(Light light, Material material, float3 position, float3 normal, float3 to_eye_direction) {
    float3 light_vector = light.position - position;
    float d = length(light_vector);

    if (d > light.falloff_end)
        return 0.0f;

    light_vector /= d;

    float ndotl = max(dot(light_vector, normal), 0.0f);
    float3 light_strength = light.color * ndotl;

    float att = attenuation(d, light.falloff_begin, light.falloff_end);
    light_strength *= att;

    return blinn_phong_model(light_strength, light_vector, normal, to_eye_direction, material);
}

float3 spot_light(Light light, Material material, float3 position, float3 normal, float3 to_eye_direction) {
    float3 light_vector = light.position - position;
    float d = length(light_vector);

    if (d > light.falloff_end)
        return 0.0f;

    light_vector /= d;

    float ndotl = max(dot(light_vector, normal), 0.0f);
    float3 light_strength = light.color * ndotl;

    float att = attenuation(d, light.falloff_begin, light.falloff_end);
    light_strength *= att;

    float spot_factor = pow(max(dot(-light_vector, light.direction), 0.0f), light.spot_power);
    light_strength *= spot_factor;

    return blinn_phong_model(light_strength, light_vector, normal, to_eye_direction, material);
}

float4 lighting(
        SceneLights scene, Material material,
        float3 position, float3 normal, float3 to_eye_direction) {
    float3 result = 0.0f;

    int i = 0;

    for( ; i < scene.directional_light_end; i++) {
        result += directional_light(scene.lights[i], material, normal, to_eye_direction);
    }

    for( ; i < scene.point_light_end; i++) {
        result += point_light(scene.lights[i], material, position, normal, to_eye_direction);
    }

    for ( ; i < scene.spot_light_end; i++) {
        result += spot_light(scene.lights[i], material, position, normal, to_eye_direction);
    }

    return float4(result, 0.0f);
}
