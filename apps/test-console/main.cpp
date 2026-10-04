// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT
#include <qce/loaders/read.hpp>

#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>

int main(int argc, char* argv[]) {
    QCE::SceneDescription scene{};
    QCE_CRITICAL(QCE::read_from_json<QCE::SceneDescription>("D:\\Project\\quadcube-engine\\apps\\common\\scenes\\game_demo.qcsd.json", scene));

    using Buffer = std::vector<uint8_t>;
    using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
    using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

    Buffer buffer;
    auto writtenSize = bitsery::quickSerialization<OutputAdapter>(buffer, scene);

    QCE::SceneDescription res{};
    auto state = bitsery::quickDeserialization<InputAdapter>(
        { buffer.begin(), writtenSize }, res);

    assert(state.first == bitsery::ReaderError::NoError && state.second);
    return 0;
}