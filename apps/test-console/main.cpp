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

int main(int argc, char* argv[]) {
    //TransformAnimation square_path{};
    //square_path.id = "square_path";
    //square_path.position_channel = {
    //    {
    //        /*value*/ { 1.0f, 0.0f, 1.0f },
    //        /*start_time*/ 0.0f
    //    },
    //    {
    //        /*value*/ { 1.0f, 0.0f, -1.0f },
    //        /*start_time*/ 1.0f
    //    },
    //    {
    //        /*value*/ { 1.0f, 1.0f, -1.0f },
    //        /*start_time*/ 2.0f
    //    },
    //    {
    //        /*value*/ { 1.0f, 1.0f, 1.0f },
    //        /*start_time*/ 3.0f
    //    },
    //    {
    //        /*value*/ { 1.0f, 0.0f, 1.0f },
    //        /*start_time*/ 4.0f
    //    }
    //};
    //square_path.scale_channel = {
    //    {
    //        /*value*/ { 1.0f, 1.0f, 1.0f },
    //        /*start_time*/ 1.0f
    //    },
    //    {
    //        /*value*/ { 1.0f, 3.0f, 1.0f },
    //        /*start_time*/ 2.0f
    //    },
    //    {
    //        /*value*/ { 3.0f, 1.0f, 1.0f },
    //        /*start_time*/ 3.0f
    //    },
    //    {
    //        /*value*/ { 1.0f, 1.0f, 1.0f },
    //        /*start_time*/ 4.0f
    //    }
    //};
    //square_path.rotation_channel = {
    //    {
    //        /*value*/ QCE::euler_deg_to_quaternion(0.0f, 0.0f, 0.0f),
    //        /*start_time*/ 0.0f
    //    },
    //    {
    //        /*value*/ QCE::euler_deg_to_quaternion(0.0f, 90.0f, 0.0f),
    //        /*start_time*/ 1.0f
    //    },
    //    {
    //        /*value*/ QCE::euler_deg_to_quaternion(0.0f, 180.0f, 0.0f),
    //        /*start_time*/ 2.0f
    //    },
    //    {
    //        /*value*/ QCE::euler_deg_to_quaternion(0.0f, 270.0f, 0.0f),
    //        /*start_time*/ 3.0f
    //    },
    //    {
    //        /*value*/ QCE::euler_deg_to_quaternion(0.0f, 0.0f, 0.0f),
    //        /*start_time*/ 4.0f
    //    }
    //};
    //square_path.total_duration = calculate_animation_duration(square_path);
    //TransformAnimation out{};

    QCE::quaternion q = { 0.1f, 0.3f, 0.3f, -0.9f };
    QCE::quaternion out{};

    Buffer buffer;
    auto written_bytes = bitsery::quickSerialization<OutputAdapter>(buffer, q);

    auto state = bitsery::quickDeserialization<InputAdapter>(
        { buffer.begin(), written_bytes }, out);

    assert(state.first == bitsery::ReaderError::NoError && state.second);
    return 0;
}