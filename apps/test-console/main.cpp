// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/loaders/binary/resources.hpp>

#include <vector>

using namespace QCE;

using Buffer = std::vector<uint8_t>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

static inline void quaternion_serialization() {
    QCE::quaternion q = { 0.1f, 0.3f, 0.3f, -0.9f };
    QCE::quaternion out{};

    Buffer buffer;
    auto written_bytes = bitsery::quickSerialization<OutputAdapter>(buffer, q);

    auto state = bitsery::quickDeserialization<InputAdapter>(
        { buffer.begin(), written_bytes }, out);

    assert(state.first == bitsery::ReaderError::NoError && state.second);
}

static inline void animation_deserialization() {
    TransformAnimation square_path{};
    square_path.id = "square_path";
    square_path.position_channel = {
        {
            /*value*/ { -6.968599f, 1.783926f, 0.204209f },
            /*start_time*/ 0.0f
        },
        {
            /*value*/ { -7.277121f, 9.905811f, 6.174850f },
            /*start_time*/ 1.0f
        },
        {
            /*value*/ { -2.614493f, 3.784512f, -4.221519f },
            /*start_time*/ 2.0f
        },
        {
            /*value*/ { 3.799920f, 9.625124f, 0.092419f },
            /*start_time*/ 3.0f
        },
        {
            /*value*/ { -6.587712f, 0.703252f, 5.143878f },
            /*start_time*/ 4.0f
        }
    };
    square_path.scale_channel = {
        {
            /*value*/ { 1.261417f, 0.237270f, 6.808356f },
            /*start_time*/ 1.0f
        },
        {
            /*value*/ { 4.929912f, 9.098186f, 5.612872f },
            /*start_time*/ 2.0f
        },
        {
            /*value*/ { 1.807719f, 2.010254f, 7.056530f },
            /*start_time*/ 3.0f
        },
        {
            /*value*/ { 2.931365f, 5.758866f, 9.438544f },
            /*start_time*/ 4.0f
        }
    };
    square_path.rotation_channel = {
        {
            /*value*/ QCE::euler_deg_to_quaternion(265.0f, 257.0f, 108.0f),
            /*start_time*/ 0.0f
        },
        {
            /*value*/ QCE::euler_deg_to_quaternion(250.0f, 15.0f, 339.0f),
            /*start_time*/ 1.0f
        },
        {
            /*value*/ QCE::euler_deg_to_quaternion(140.0f, 107.0f, 308.0f),
            /*start_time*/ 2.0f
        },
        {
            /*value*/ QCE::euler_deg_to_quaternion(314.0f, 178.0f, 319.0f),
            /*start_time*/ 3.0f
        },
        {
            /*value*/ QCE::euler_deg_to_quaternion(51.0f, 252.0f, 4.0f),
            /*start_time*/ 4.0f
        }
    };
    square_path.total_duration = calculate_animation_duration(square_path);

    TransformAnimationCompressionContext ctx = calculate_compression_context(square_path);
    (void)ctx;

    Buffer buffer;
    auto written_bytes = bitsery::quickSerialization<OutputAdapter>(buffer, square_path);

    TransformAnimation out{};
    auto state = bitsery::quickDeserialization<InputAdapter>(
        { buffer.begin(), written_bytes }, out);

    assert(state.first == bitsery::ReaderError::NoError && state.second);
}

int main(int argc, char* argv[]) {
    quaternion_serialization();
    animation_deserialization();
    return 0;
}