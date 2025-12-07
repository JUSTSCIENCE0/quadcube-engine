// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/error_codes.hpp>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(CommandContextType) \
        CU_ENUM_UNIT(E_CCT_HID_EVENT) \
    CU_END_ENUM(CommandContextType)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    struct CommandContext {
        CommandContext(CommandContextType cct):
            type(cct) {}

        const CommandContextType type =
            CommandContextType::E_CommandContextType_UNKNOWN;
    };

    struct Command {
        explicit Command(std::string name) :
            m_name(std::move(name)) {}
        Command(const Command&) = delete;
        Command(Command&&) = delete;
        Command& operator=(const Command&) = delete;
        Command& operator=(Command&&) = delete;
        virtual ~Command() = default;

        virtual ErrorCode Execute(const CommandContext* context) = 0;

        const std::string m_name;
    };

    // default commands
    struct ExitCommand : public Command {
        ExitCommand() :
            Command("Exit") {};

        ErrorCode Execute(const CommandContext* context) {
            std::exit(0);
        }
    };
}
