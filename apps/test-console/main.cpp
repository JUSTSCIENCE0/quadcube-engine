// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT
#include <qce/loaders/read.hpp>

int main(int argc, char* argv[]) {
    QCE::SceneDescription scene{};
    QCE_SOFT(QCE::read_from_json<QCE::SceneDescription>("D:\\Project\\quadcube-engine\\apps\\common\\scenes\\game_demo.qcsd.json", scene));

    return 0;
}