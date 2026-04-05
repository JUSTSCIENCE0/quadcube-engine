// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// camera config
// TODO

// hid config
// TODO

// movement config
MJSON_OBJECT_BEGIN(
        MovementConfig,
        "Movement Config",
        "Configuration of the movement system")
    MJSON_FIELD(std::optional<std::string>, temporal, nullptr, nullptr)
MJSON_OBJECT_END(MovementConfig)

// render config
// TODO


