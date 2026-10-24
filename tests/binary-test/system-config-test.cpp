// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <gtest/gtest.h>

#include <qce/loaders/binary/system_configs.hpp>

#include <bitsery/adapter/buffer.h>

using namespace QCE;

using Buffer = std::vector<uint8_t>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

template<typename T>
void check_round_trip(const T& source, T& deserialized) {
    Buffer buffer;
    const auto written_bytes =
        bitsery::quickSerialization(OutputAdapter{ buffer }, source);
    ASSERT_GT(written_bytes, 0);

    const auto state = bitsery::quickDeserialization<InputAdapter>(
        { buffer.begin(), written_bytes }, deserialized);
    ASSERT_EQ(state.first, bitsery::ReaderError::NoError);
    ASSERT_TRUE(state.second);
}

TEST(SystemConfigSerializationTest, SerializesEveryConfigAndEventType) {
    CameraConfigUnit camera_unit{};
    camera_unit.aspect = 1.5f;
    camera_unit.fov_rad = 0.75f;
    camera_unit.znear = 0.25f;
    camera_unit.zfar = 500.0f;
    camera_unit.position = { 1.0f, 2.0f, 3.0f };
    camera_unit.target = { 4.0f, 5.0f, 6.0f };
    camera_unit.up = { 0.0f, 1.0f, 0.0f };
    camera_unit.camera_name = "test-camera";
    camera_unit.camera_type = CameraType::E_CAMERA_THIRD_PERSON;
    CameraConfigUnit decoded_camera_unit{};
    check_round_trip(camera_unit, decoded_camera_unit);
    EXPECT_FLOAT_EQ(decoded_camera_unit.aspect, camera_unit.aspect);
    EXPECT_FLOAT_EQ(decoded_camera_unit.fov_rad, camera_unit.fov_rad);
    EXPECT_FLOAT_EQ(decoded_camera_unit.znear, camera_unit.znear);
    EXPECT_FLOAT_EQ(decoded_camera_unit.zfar, camera_unit.zfar);
    EXPECT_EQ(decoded_camera_unit.position, camera_unit.position);
    EXPECT_EQ(decoded_camera_unit.target, camera_unit.target);
    EXPECT_EQ(decoded_camera_unit.up, camera_unit.up);
    EXPECT_EQ(decoded_camera_unit.camera_name, camera_unit.camera_name);
    EXPECT_EQ(decoded_camera_unit.camera_type, camera_unit.camera_type);

    CameraConfig camera_config{};
    camera_config.cameras.push_back(camera_unit);
    CameraConfig decoded_camera_config{};
    check_round_trip(camera_config, decoded_camera_config);
    ASSERT_EQ(decoded_camera_config.cameras.size(), 1);
    EXPECT_EQ(decoded_camera_config.cameras.front().camera_name, camera_unit.camera_name);

    HidEventDescriptor event_descriptor{};
    event_descriptor.type = HidEventType::E_HET_ACCORD;
    event_descriptor.handler = "base-handler";
    HidEventDescriptor decoded_event_descriptor{};
    check_round_trip(event_descriptor, decoded_event_descriptor);
    EXPECT_EQ(decoded_event_descriptor.type, event_descriptor.type);
    EXPECT_EQ(decoded_event_descriptor.handler, event_descriptor.handler);

    HidSingleEvent single_event{};
    single_event.handler = "single-handler";
    single_event.hid_event_code_name = "E_HEC_KEYBOARD_SPACE";
    HidSingleEvent decoded_single_event{};
    check_round_trip(single_event, decoded_single_event);
    EXPECT_EQ(decoded_single_event.type, HidEventType::E_HET_SINGLE);
    EXPECT_EQ(decoded_single_event.handler, single_event.handler);
    EXPECT_EQ(decoded_single_event.hid_event_code_name, single_event.hid_event_code_name);

    HidConfig hid_config{};
    auto configured_event = std::make_unique<HidSingleEvent>();
    configured_event->handler = single_event.handler;
    configured_event->hid_event_code_name = single_event.hid_event_code_name;
    hid_config.event_descriptors.push_back(std::move(configured_event));
    auto base_event = std::make_unique<HidEventDescriptor>();
    base_event->type = HidEventType::E_HET_COMBO;
    base_event->handler = "combo-handler";
    hid_config.event_descriptors.push_back(std::move(base_event));
    HidConfig decoded_hid_config{};
    check_round_trip(hid_config, decoded_hid_config);
    ASSERT_EQ(decoded_hid_config.event_descriptors.size(), 2);
    auto* decoded_configured_event =
        dynamic_cast<HidSingleEvent*>(decoded_hid_config.event_descriptors.front().get());
    ASSERT_NE(decoded_configured_event, nullptr);
    EXPECT_EQ(decoded_configured_event->handler, single_event.handler);
    EXPECT_EQ(decoded_configured_event->hid_event_code_name, single_event.hid_event_code_name);
    EXPECT_EQ(decoded_hid_config.event_descriptors.back()->type, HidEventType::E_HET_COMBO);
    EXPECT_EQ(decoded_hid_config.event_descriptors.back()->handler, "combo-handler");

    MovementConfig movement_config{};
    movement_config.temporal = "walk";
    MovementConfig decoded_movement_config{};
    check_round_trip(movement_config, decoded_movement_config);
    EXPECT_EQ(decoded_movement_config.temporal, movement_config.temporal);

    ShaderDescr shader_description{};
    shader_description.shader_name = "test-shader";
    shader_description.shader_type = ShaderType::E_PIXEL_SHADER;
    ShaderDescr decoded_shader_description{};
    check_round_trip(shader_description, decoded_shader_description);
    EXPECT_EQ(decoded_shader_description.shader_name, shader_description.shader_name);
    EXPECT_EQ(decoded_shader_description.shader_type, shader_description.shader_type);

    RenderConfig render_config{};
    render_config.width = 1920;
    render_config.height = 1080;
    render_config.used_shaders.push_back(shader_description);
    RenderConfig decoded_render_config{};
    check_round_trip(render_config, decoded_render_config);
    EXPECT_EQ(decoded_render_config.width, render_config.width);
    EXPECT_EQ(decoded_render_config.height, render_config.height);
    ASSERT_EQ(decoded_render_config.used_shaders.size(), 1);
    EXPECT_EQ(decoded_render_config.used_shaders.front().shader_name, shader_description.shader_name);

    GraphicsOutputConfig graphics_config{};
    graphics_config.mode = WindowMode::E_FULLSCREEN_WINDOW;
    graphics_config.width = 1920;
    graphics_config.height = 1080;
    graphics_config.caption = "test-window";
    graphics_config.is_first_person = true;
    GraphicsOutputConfig decoded_graphics_config{};
    check_round_trip(graphics_config, decoded_graphics_config);
    EXPECT_EQ(decoded_graphics_config.mode, graphics_config.mode);
    EXPECT_EQ(decoded_graphics_config.width, graphics_config.width);
    EXPECT_EQ(decoded_graphics_config.height, graphics_config.height);
    EXPECT_EQ(decoded_graphics_config.caption, graphics_config.caption);
    EXPECT_EQ(decoded_graphics_config.is_first_person, graphics_config.is_first_person);

    CommonConfig common_config{};
    common_config.render_type = RenderType::E_RENDER_DIRECTX12;
    CommonConfig decoded_common_config{};
    check_round_trip(common_config, decoded_common_config);
    EXPECT_EQ(decoded_common_config.render_type, common_config.render_type);
}
