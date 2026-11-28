// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <gtest/gtest.h>

#include <qce/loaders/binary/gltf.hpp>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <vector>

static constexpr std::uint8_t PNG_IMAGE[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a,
    0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0x15, 0xc4,
    0x89, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x44, 0x41,
    0x54, 0x78, 0xda, 0x63, 0xf8, 0xcf, 0xc0, 0xf0,
    0x1f, 0x00, 0x05, 0x00, 0x01, 0xff, 0x56, 0xc7,
    0x2f, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
    0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

class GltfLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
        m_gltf_path = std::filesystem::temp_directory_path() /
            ("qce_gltf_texture_" + std::to_string(timestamp) + ".gltf");
        m_image_path = m_gltf_path;
        m_image_path.replace_extension(".png");

        std::ofstream image_file(m_image_path, std::ios::binary);
        ASSERT_TRUE(image_file);
        image_file.write(reinterpret_cast<const char*>(PNG_IMAGE), sizeof(PNG_IMAGE));
        ASSERT_TRUE(image_file);

        std::ofstream gltf_file(m_gltf_path);
        ASSERT_TRUE(gltf_file);
        gltf_file
            << R"({"asset":{"version":"2.0"},"materials":[{"pbrMetallicRoughness":{"baseColorTexture":{"index":0}}}],"textures":[{"source":0}],"images":[{"uri":")"
            << m_image_path.filename().string()
            << R"("}]})";
        ASSERT_TRUE(gltf_file);
    }

    void TearDown() override {
        std::error_code error;
        std::filesystem::remove(m_gltf_path, error);
        std::filesystem::remove(m_image_path, error);
    }

    std::filesystem::path m_gltf_path;
    std::filesystem::path m_image_path;
};

TEST_F(GltfLoaderTest, ImportsExternalBaseColorTextureAndAssignsItToMaterial) {
    QCE::ModelResources resources;
    std::vector<QCE::ModelDescription> model_descriptions;
    ASSERT_EQ(
        QCE::load_gltf_file(m_gltf_path, resources, model_descriptions),
        QCE::ErrorCode::SUCCESS);

    ASSERT_EQ(resources.materials.size(), 1u);
    ASSERT_EQ(resources.textures.size(), 1u);
    ASSERT_TRUE(resources.materials[0].albedo_texture.has_value());
    EXPECT_EQ(resources.materials[0].albedo_texture.value(), 0u);
    EXPECT_EQ(resources.textures[0].base_width, 1u);
    EXPECT_EQ(resources.textures[0].base_height, 1u);
}
