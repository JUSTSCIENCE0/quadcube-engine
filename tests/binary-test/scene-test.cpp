// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <gtest/gtest.h>

#include <qce/loaders/binary/scene.hpp>
#include <qce/loaders/read.hpp>

#include <math-utils.hpp>

#include <type_traits>
#include <vector>

using namespace QCE;

using Buffer = std::vector<uint8_t>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

SceneDescription generate_full_sample_scene() {
    SceneDescription scene{};

    MaterialParams material{};
    material.albedo_color = { 0.2f, 0.4f, 0.6f, 1.0f };
    material.fresnel = { 0.1f, 0.2f, 0.3f };
    material.shininess = 0.85f;
    material.albedo_texture = "albedo";
    material.normal_texture = "normal";
    material.roughness_texture = "roughness";
    material.emissive_texture = "emissive";
    MaterialParams file_material{
        .load_from_file = true,
        .albedo_color = std::nullopt,
        .fresnel = std::nullopt,
        .shininess = std::nullopt
    };

    scene.resources = {
        { "cuboid", CuboidParams{ 2.0f, 3.0f, 4.0f } },
        { "sphere", SphereParams{ 1.5f, 8, true } },
        { "plane", PlaneParams{ 5.0f, 6.0f, true, true, false } },
        { "mesh", MeshParams{ true } },
        { "texture", TextureParams{ E_TEXTYPE_CUBE_MAP } },
        { "material", material },
        { "file_material", file_material },
        { "animation", AnimationParams{ AnimationType::E_ANIMATION_TRANSFORM } }
    };

    EntityDescription entity{};
    entity.name = "entity";
    entity.static_mesh = "mesh";
    entity.dynamic_mesh = DynamicMeshDescription{ "base", "deformed" };
    entity.material = "material";

    DirectionalLight directional_light{};
    directional_light.color = { 0.2f, 0.3f, 0.4f, 1.0f };
    directional_light.direction = { 1.0f, -2.0f, 3.0f };
    entity.directional_light = directional_light;

    PointLight point_light{};
    point_light.color = { 0.4f, 0.5f, 0.6f, 1.0f };
    point_light.position = { 4.0f, 5.0f, 6.0f };
    point_light.falloff_begin = 2.0f;
    point_light.falloff_end = 20.0f;
    entity.point_light = point_light;

    SpotLight spot_light{};
    spot_light.color = { 0.6f, 0.7f, 0.8f, 1.0f };
    spot_light.position = { 7.0f, 8.0f, 9.0f };
    spot_light.direction = { -1.0f, -2.0f, -3.0f };
    spot_light.falloff_begin = 3.0f;
    spot_light.falloff_end = 30.0f;
    spot_light.spot_power = 16.0f;
    entity.spot_light = spot_light;

    TransformComponents transform{};
    transform.rotation = euler_deg_to_quaternion(20.0f, 35.0f, 50.0f);
    transform.position = { 10.0f, 11.0f, 12.0f };
    transform.scale = { 1.5f, 2.0f, 2.5f };
    entity.transform = transform;
    entity.transform_animation = TransformAnimationDescription{ "walk", true };

    scene.entities = { entity, EntityDescription{} };
    return scene;
}

SceneDescription full_sample_scene = generate_full_sample_scene();
SceneDescription empty_scene{};

class SceneDescriptionSerializationTest :
        public ::testing::TestWithParam<const SceneDescription*> {
protected:
    void Process() {
        const auto& source = *GetParam();

        Buffer buffer;
        const auto written_bytes = bitsery::quickSerialization(OutputAdapter{ buffer }, source);
        ASSERT_GT(written_bytes, 0) << "Failed to serialize scene description";

        auto state = bitsery::quickDeserialization<InputAdapter>(
            { buffer.begin(), written_bytes }, deserialized);
        ASSERT_EQ(state.first, bitsery::ReaderError::NoError);
        ASSERT_TRUE(state.second) << "Not all data in the buffer was deserialized";
    }

    void Validate() {
        const auto& source = *GetParam();

        ASSERT_EQ(source.resources.size(), deserialized.resources.size());
        for (size_t i = 0; i < source.resources.size(); ++i) {
            const auto& src = source.resources[i];
            const auto& des = deserialized.resources[i];

            EXPECT_EQ(src.name, des.name);
            ASSERT_EQ(src.params.index(), des.params.index());
            std::visit([&](const auto& src_params) {
                using T = std::decay_t<decltype(src_params)>;
                const auto& des_params = std::get<T>(des.params);

                if constexpr (std::is_same_v<T, CuboidParams>) {
                    EXPECT_FLOAT_EQ(src_params.length, des_params.length);
                    EXPECT_FLOAT_EQ(src_params.width, des_params.width);
                    EXPECT_FLOAT_EQ(src_params.height, des_params.height);
                }
                else if constexpr (std::is_same_v<T, SphereParams>) {
                    EXPECT_FLOAT_EQ(src_params.radius, des_params.radius);
                    EXPECT_EQ(src_params.subdivisions, des_params.subdivisions);
                    EXPECT_EQ(src_params.hard_edges, des_params.hard_edges);
                }
                else if constexpr (std::is_same_v<T, PlaneParams>) {
                    EXPECT_FLOAT_EQ(src_params.length, des_params.length);
                    EXPECT_FLOAT_EQ(src_params.width, des_params.width);
                    EXPECT_EQ(src_params.hard_edges, des_params.hard_edges);
                    EXPECT_EQ(src_params.repeat_uv, des_params.repeat_uv);
                    EXPECT_EQ(src_params.unit_squares, des_params.unit_squares);
                }
                else if constexpr (std::is_same_v<T, MeshParams>) {
                    EXPECT_EQ(src_params.is_empty, des_params.is_empty);
                }
                else if constexpr (std::is_same_v<T, TextureParams>) {
                    EXPECT_EQ(src_params.texture_type, des_params.texture_type);
                }
                else if constexpr (std::is_same_v<T, MaterialParams>) {
                    EXPECT_EQ(src_params.load_from_file, des_params.load_from_file);
                    ASSERT_EQ(src_params.albedo_color.has_value(), des_params.albedo_color.has_value());
                    ASSERT_EQ(src_params.fresnel.has_value(), des_params.fresnel.has_value());
                    ASSERT_EQ(src_params.shininess.has_value(), des_params.shininess.has_value());
                    if (src_params.albedo_color.has_value())
                        test::check_color_rgba(src_params.albedo_color.value(), des_params.albedo_color.value());
                    if (src_params.fresnel.has_value())
                        test::check_float3d(src_params.fresnel.value(), des_params.fresnel.value(), 0.000001f);
                    if (src_params.shininess.has_value())
                        EXPECT_FLOAT_EQ(src_params.shininess.value(), des_params.shininess.value());
                    EXPECT_EQ(src_params.albedo_texture, des_params.albedo_texture);
                    EXPECT_EQ(src_params.normal_texture, des_params.normal_texture);
                    EXPECT_EQ(src_params.metallic_texture, des_params.metallic_texture);
                    EXPECT_EQ(src_params.roughness_texture, des_params.roughness_texture);
                    EXPECT_EQ(src_params.occlusion_texture, des_params.occlusion_texture);
                    EXPECT_EQ(src_params.emissive_texture, des_params.emissive_texture);
                }
                else if constexpr (std::is_same_v<T, AnimationParams>) {
                    EXPECT_EQ(src_params.animation_type, des_params.animation_type);
                }
                }, src.params);
        }

        ASSERT_EQ(source.entities.size(), deserialized.entities.size());
        for (size_t i = 0; i < source.entities.size(); ++i) {
            const auto& src = source.entities[i];
            const auto& des = deserialized.entities[i];

            EXPECT_EQ(src.name, des.name);
            EXPECT_EQ(src.static_mesh, des.static_mesh);
            EXPECT_EQ(src.material, des.material);
            EXPECT_EQ(src.dynamic_mesh.has_value(), des.dynamic_mesh.has_value());
            if (src.dynamic_mesh && des.dynamic_mesh) {
                EXPECT_EQ(src.dynamic_mesh->base, des.dynamic_mesh->base);
                EXPECT_EQ(src.dynamic_mesh->deformated, des.dynamic_mesh->deformated);
            }

            EXPECT_EQ(src.directional_light.has_value(), des.directional_light.has_value());
            if (src.directional_light && des.directional_light) {
                test::check_color_rgba(src.directional_light->color, des.directional_light->color);
                test::check_float3d(src.directional_light->direction, des.directional_light->direction, 0.000001f);
            }

            EXPECT_EQ(src.point_light.has_value(), des.point_light.has_value());
            if (src.point_light && des.point_light) {
                test::check_color_rgba(src.point_light->color, des.point_light->color);
                test::check_float3d(src.point_light->position, des.point_light->position, 0.000001f);
                EXPECT_FLOAT_EQ(src.point_light->falloff_begin, des.point_light->falloff_begin);
                EXPECT_FLOAT_EQ(src.point_light->falloff_end, des.point_light->falloff_end);
            }

            EXPECT_EQ(src.spot_light.has_value(), des.spot_light.has_value());
            if (src.spot_light && des.spot_light) {
                test::check_color_rgba(src.spot_light->color, des.spot_light->color);
                test::check_float3d(src.spot_light->position, des.spot_light->position, 0.000001f);
                test::check_float3d(src.spot_light->direction, des.spot_light->direction, 0.000001f);
                EXPECT_FLOAT_EQ(src.spot_light->falloff_begin, des.spot_light->falloff_begin);
                EXPECT_FLOAT_EQ(src.spot_light->falloff_end, des.spot_light->falloff_end);
                EXPECT_FLOAT_EQ(src.spot_light->spot_power, des.spot_light->spot_power);
            }

            EXPECT_EQ(src.transform.has_value(), des.transform.has_value());
            if (src.transform && des.transform) {
                test::check_quaternions(
                    src.transform->rotation, des.transform->rotation, E_32_BIT_QQ);
                test::check_float3d(src.transform->position, des.transform->position, 0.000001f);
                test::check_float3d(src.transform->scale, des.transform->scale, 0.000001f);
            }

            EXPECT_EQ(src.transform_animation.has_value(), des.transform_animation.has_value());
            if (src.transform_animation && des.transform_animation) {
                EXPECT_EQ(src.transform_animation->animation_name, des.transform_animation->animation_name);
                EXPECT_EQ(src.transform_animation->is_looped, des.transform_animation->is_looped);
            }
        }
    }

private:
    SceneDescription deserialized{};
};

TEST_P(SceneDescriptionSerializationTest, SerializeDeserialize) {
    Process();
    Validate();
}

TEST(SceneDescriptionJsonTest, MaterialFileFlagAllowsInlineParametersToBeOmitted) {
    rapidjson::Document json;
    json.Parse(R"({
        "resources": [
            {
                "name": "material",
                "params": {
                    "type": "material",
                    "load_from_file": true
                }
            }
        ]
    })");
    ASSERT_FALSE(json.HasParseError());

    SceneDescription scene{};
    ASSERT_EQ(macrojson::read_from_json(nullptr, json, scene), macrojson::E_MJSON_OK);
    ASSERT_EQ(scene.resources.size(), 1);
    ASSERT_TRUE(std::holds_alternative<MaterialParams>(scene.resources.front().params));

    const auto& params = std::get<MaterialParams>(scene.resources.front().params);
    EXPECT_EQ(params.load_from_file, true);
    EXPECT_FALSE(params.albedo_color.has_value());
    EXPECT_FALSE(params.fresnel.has_value());
    EXPECT_FALSE(params.shininess.has_value());
}

TEST(MaterialSerializationTest, SerializesMaterialResource) {
    Material source{};
    source.id = "material";
    source.albedo_color = { 0.2f, 0.4f, 0.6f, 1.0f };
    source.fresnel = { 0.1f, 0.2f, 0.3f };
    source.shininess = 0.85f;
    source.albedo_texture = 1;
    source.normal_texture = 2;
    source.metallic_texture = 3;
    source.roughness_texture = 4;
    source.occlusion_texture = 5;
    source.emissive_texture = 6;

    Buffer buffer;
    const auto written_bytes = bitsery::quickSerialization(OutputAdapter{ buffer }, source);
    ASSERT_GT(written_bytes, 0);

    Material deserialized{};
    const auto state = bitsery::quickDeserialization<InputAdapter>(
        { buffer.begin(), written_bytes }, deserialized);
    ASSERT_EQ(state.first, bitsery::ReaderError::NoError);
    ASSERT_TRUE(state.second);

    EXPECT_EQ(deserialized.id, source.id);
    test::check_color_rgba(source.albedo_color, deserialized.albedo_color);
    test::check_float3d(source.fresnel, deserialized.fresnel, 0.000001f);
    EXPECT_FLOAT_EQ(deserialized.shininess, source.shininess);
    EXPECT_EQ(deserialized.albedo_texture, source.albedo_texture);
    EXPECT_EQ(deserialized.normal_texture, source.normal_texture);
    EXPECT_EQ(deserialized.metallic_texture, source.metallic_texture);
    EXPECT_EQ(deserialized.roughness_texture, source.roughness_texture);
    EXPECT_EQ(deserialized.occlusion_texture, source.occlusion_texture);
    EXPECT_EQ(deserialized.emissive_texture, source.emissive_texture);
}

INSTANTIATE_TEST_SUITE_P(
    Samples,
    SceneDescriptionSerializationTest,
    ::testing::Values(&full_sample_scene, &empty_scene));
