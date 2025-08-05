// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#define CLI_CONFIGURATION \
    CLI_OPTIONAL_PROPERTY(operation, WO_SYMBOL, operation, "operation to be performed on the data", \
        std::string, "None", ListValidator, \
        "None", \
        "VectorAddition", "VectorSubtraction", "VectorMultiplication", "VectorDivision", \
        "VectorScalarMultiplication", "VectorScalarDivision", "VectorLength", \
        "VectorNormalization", "VectorDotProduct", "VectorCrossProduct", "VectorMatrixMultiplication", \
        "MatrixAddition", "MatrixSubtraction", "MatrixMultiplication", "MatrixTransposition", \
        "MatrixDeterminant", "MatrixInvert", "MatrixVectorMultiplication") \
    CLI_OPTIONAL_PROPERTY(input-bin-file, WO_SYMBOL, input_bin_file, "path to binary file containing float32 array", \
        std::filesystem::path, "", BaseValidator) \
    CLI_OPTIONAL_PROPERTY(input-txt-file, WO_SYMBOL, input_txt_file, "path to text file containing float array", \
        std::filesystem::path, "", BaseValidator) \
    CLI_OPTIONAL_PROPERTY(output-bin-file, WO_SYMBOL, output_bin_file, "path to the binary file where the result will be written", \
        std::filesystem::path, "", BaseValidator) \
    CLI_OPTIONAL_PROPERTY(output-txt-file, WO_SYMBOL, output_txt_file, "path to the text file where the result will be written", \
        std::filesystem::path, "", BaseValidator) \
    CLI_FLAG(to-stdout, WO_SYMBOL, is_stdout, "duplicate result in stdout") \
    CLI_OPTIONAL_PROPERTY(simd-acceleration, WO_SYMBOL, simd_acceleration, "use the specified SIMD acceleration " \
        "(if not specified, the acceleration type will be selected automatically for the current hardware)", \
        std::string, "AUTO", ListValidator, "AUTO", "DEF", "SSE2", "AVX2", "AVX512F" ) \
    CLI_FLAG(print-hardware-info, WO_SYMBOL, print_hardware_info, "display description of current hardware") \


#define CLI_ABOUT \
    "QuadCube Math Demo\n" \
    "Copyright (c) 2025, Yakov Usoltsev\n" \
    "Email: yakovmen62@gmail.com\n" \
    "License: MIT"

#include "calculator.hpp"

#include <cu/cli-utils.hpp>
#include <cu/cpu-utils.hpp>
#include <cu/file-utils.hpp>

int main(int argc, char* argv[]) {
    CU::CLIConfig cli_config{};
    if (!CU::parse_cli_args(argc, argv, &cli_config))
        return -1;

    if (cli_config.print_hardware_info) {
        std::cout << CU::CURRENT_CPU_CONFIGURATION << std::endl;
    }

    std::vector<float> input_data;

    if (!cli_config.input_bin_file.empty()) {
        input_data = CU::load_data_from_file<float>(cli_config.input_bin_file);
    }
    else if (!cli_config.input_txt_file.empty()) {
        input_data = CU::load_data_from_text_file<float>(cli_config.input_txt_file);
    }

    if (input_data.empty()) {
        std::cout << "Error: Input data not found!" << std::endl;
        return -1;
    }

    std::cout << "Info: Loaded " << input_data.size() << " floats" << std::endl;

    auto calculator = QCEMathDemo::make_calculator(CU::AUTO_INSET, input_data);
    if (nullptr == calculator) {
        std::cout << "Failed to make selected implementation" << std::endl;
        return -1;
    }

    std::cout << calculator->Description() << std::endl;

    return 0;
}