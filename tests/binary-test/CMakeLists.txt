# Copyright (c) 2026, Yakov Usoltsev
# Email: yakovmen62@gmail.com
#
# License: MIT

cmake_minimum_required(VERSION 3.22)

project(binary-test)

add_executable(${PROJECT_NAME}
    animation-test.cpp

    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        quadcube-engine
)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests")
target_interface_group(common-utils)
target_interface_group(quadcube-engine)