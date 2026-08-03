// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/binary/math.hpp>

#include <vector>

using namespace QCE;

using Buffer = std::vector<uint8_t>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

int main(int argc, char* argv[]) {
    float4d value = { 1.0f, 2.0f, 3.0f, 4.0f };
    float4d deser;

    Buffer buffer;
    auto written_bytes = bitsery::quickSerialization<OutputAdapter>(buffer, value);

    auto state = bitsery::quickDeserialization<InputAdapter>(
        { buffer.begin(), written_bytes }, deser);

    assert(state.first == bitsery::ReaderError::NoError && state.second);
    return 0;
}