# Copyright (c) 2025, Yakov Usoltsev
# Email: yakovmen62@gmail.com
#
# License: MIT

cmake_minimum_required(VERSION 3.22)

project(math-test)

add_executable(${PROJECT_NAME}
    vector_operations_conformance.cpp
    vector_operations_performance.cpp
    vector_operations_iface.hpp
    vector_operations_impl.hpp

    matrix_operations_conformance.cpp
    matrix_operations_performance.cpp
    matrix_operations_iface.hpp
    matrix_operations_impl.hpp

    main.cpp
)

include(SimdAutogenerator)
generate_simd_compile_units(${PROJECT_NAME} vector_operations DEF SSE2)
generate_simd_compile_units(${PROJECT_NAME} matrix_operations DEF SSE2)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        quadcube-engine
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        QCE_TEST_DATA_PATH="${QCE_TEST_DATA_PATH}"
    )

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests")
target_interface_group(common-utils)
target_interface_group(quadcube-engine)
