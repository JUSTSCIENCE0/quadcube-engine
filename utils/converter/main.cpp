// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#define CLI_CONFIGURATION \
    CLI_REQUIRED_PROPERTY(input, SYMBOL(i), input_file, "input file path", \
        std::string, BaseValidator) \
    CLI_REQUIRED_PROPERTY(output, SYMBOL(o), output_file, "output file path", \
        std::string, BaseValidator) \
    CLI_REQUIRED_PROPERTY(convertion, SYMBOL(c), convertion, "convertion type", \
        std::string, ListValidator, "to_json", "to_bin" ) \
    CLI_OPTIONAL_PROPERTY(format, SYMBOL(f), format, "input file format", \
        std::string, "auto", ListValidator, "autodetect", "animation" )

#define CLI_ABOUT \
    "Copyright (c) 2026, Yakov Usoltsev\n" \
    "Email: yakovmen62@gmail.com\n" \
    "License: MIT"

#include <cu/cli-utils.hpp>

#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(Convertion) \
        CU_ENUM_UNIT(to_json) \
        CU_ENUM_UNIT(to_bin) \
    CU_END_ENUM(Convertion) \
    CU_BEGIN_ENUM(Format) \
        CU_ENUM_UNIT(autodetect) \
        CU_ENUM_UNIT(animation) \
    CU_END_ENUM(Format)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

int main(int argc, char* argv[]) {
    CU::CLIConfig cli_config{};
    if (!CU::parse_cli_args(argc, argv, &cli_config))
        return -1;

    std::cout << cli_config << std::endl;

    return 0;
}
