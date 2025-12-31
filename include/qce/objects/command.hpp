// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/error_codes.hpp>

#include <memory>

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

    struct BaseCommand {
        explicit BaseCommand(std::string name) :
            m_name(std::move(name)) {}
        BaseCommand(const BaseCommand&) = delete;
        BaseCommand(BaseCommand&&) = delete;
        BaseCommand& operator=(const BaseCommand&) = delete;
        BaseCommand& operator=(BaseCommand&&) = delete;
        virtual ~BaseCommand() = default;

        virtual ErrorCode Execute(const CommandContext* context) = 0;

        const std::string m_name;
    };

    struct Command {
        std::string id;
        std::shared_ptr<BaseCommand> command;
    };

    // default commands
    struct ExitCommand : public BaseCommand {
        ExitCommand() :
            BaseCommand("Exit") {};

        ErrorCode Execute(const CommandContext* context) {
            std::exit(0);
        }
    };
}
