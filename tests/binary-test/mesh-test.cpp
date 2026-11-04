// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <gtest/gtest.h>

#include <qce/loaders/binary/resources.hpp>

#include <vector>

using namespace QCE;

using Buffer = std::vector<uint8_t>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

static Mesh generate_sample_mesh() {
    Mesh mesh{};
    mesh.id = "triangle";
    mesh.vertices = {
        {
            .position = { 0.0f, 1.0f, 0.0f },
            .normal = { 0.0f, 0.0f, 1.0f },
            .texture_coordinates = { { 0.5f, 1.0f } }
        },
        {
            .position = { -1.0f, -1.0f, 0.0f },
            .normal = { 0.0f, 0.0f, 1.0f },
            .texture_coordinates = { { 0.0f, 0.0f } }
        },
        {
            .position = { 1.0f, -1.0f, 0.0f },
            .normal = { 0.0f, 0.0f, 1.0f },
            .texture_coordinates = { { 1.0f, 0.0f } }
        }
    };
    mesh.indices = { 0, 1, 2 };
    return mesh;
}

static Mesh sample_mesh = generate_sample_mesh();
static Mesh empty_mesh{};

class MeshSerializationTest : public ::testing::TestWithParam<const Mesh*> {
protected:
    void Process() {
        Buffer buffer;
        const auto written_bytes = bitsery::quickSerialization(OutputAdapter{ buffer }, *GetParam());
        ASSERT_GT(written_bytes, 0) << "Failed to serialize mesh";

        const auto state = bitsery::quickDeserialization<InputAdapter>(
            { buffer.begin(), written_bytes }, deserialized);
        ASSERT_EQ(state.first, bitsery::ReaderError::NoError);
        ASSERT_TRUE(state.second) << "Not all data in the buffer was deserialized";
    }

    void Validate() {
        const auto& source = *GetParam();

        EXPECT_EQ(source.id, deserialized.id);
        ASSERT_EQ(source.vertices.size(), deserialized.vertices.size());
        for (size_t i = 0; i < source.vertices.size(); ++i) {
            const auto& src = source.vertices[i];
            const auto& des = deserialized.vertices[i];
            EXPECT_FLOAT_EQ(src.position.x(), des.position.x());
            EXPECT_FLOAT_EQ(src.position.y(), des.position.y());
            EXPECT_FLOAT_EQ(src.position.z(), des.position.z());
            EXPECT_FLOAT_EQ(src.normal.x(), des.normal.x());
            EXPECT_FLOAT_EQ(src.normal.y(), des.normal.y());
            EXPECT_FLOAT_EQ(src.normal.z(), des.normal.z());
            EXPECT_FLOAT_EQ(src.texture_coordinates.x(), des.texture_coordinates.x());
            EXPECT_FLOAT_EQ(src.texture_coordinates.y(), des.texture_coordinates.y());
        }

        EXPECT_EQ(source.indices, deserialized.indices);
    }

private:
    Mesh deserialized{};
};

TEST_P(MeshSerializationTest, SerializeDeserialize) {
    Process();
    Validate();
}

INSTANTIATE_TEST_SUITE_P(
    Samples,
    MeshSerializationTest,
    ::testing::Values(&sample_mesh, &empty_mesh));
