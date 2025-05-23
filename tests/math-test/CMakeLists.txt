# Copyright (c) 2025, Yakov Usoltsev
# Email: yakovmen62@gmail.com
#
# License: MIT

cmake_minimum_required(VERSION 3.22)

project(math-test)

add_executable(${PROJECT_NAME}
    vector_operations_conformance.cpp
    vector_operations_template.hpp
    vector_operations_def.cpp
    vector_operations.hpp

    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        GTest::gtest
        quadcube-engine
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        QCE_TEST_DATA_PATH="${QCE_TEST_DATA_PATH}"
    )

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests")
target_interface_group(common-utils)
target_interface_group(quadcube-engine)
