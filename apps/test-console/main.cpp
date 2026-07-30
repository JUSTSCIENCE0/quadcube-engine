// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/mjson/generated_objects.hpp>

using namespace QCE;

int main(int argc, char* argv[]) {
    AnimationPositionKey pos {
        .value = { 1.0f, 0.0f, 1.0f },
        .start_time = 0.0f
    };

    std::string json{};
    macrojson::object_to_json_str(pos, json);

    std::cout << "pos" << std::endl;
    std::cout << "Serialized JSON:" << std::endl;
    std::cout << json << std::endl;
    std::cout << std::endl;

    return 0;
}