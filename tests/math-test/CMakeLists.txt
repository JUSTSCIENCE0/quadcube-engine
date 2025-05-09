# Copyright (c) 2025, Yakov Usoltsev
# Email: yakovmen62@gmail.com
#
# License: MIT

cmake_minimum_required(VERSION 3.22)

project(math-test)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        GTest::gtest
        quadcube-engine
)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests")
