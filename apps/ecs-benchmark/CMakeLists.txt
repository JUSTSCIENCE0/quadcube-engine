# Copyright (c) 2025, Yakov Usoltsev
# Email: yakovmen62@gmail.com
#
# License: MIT

cmake_minimum_required(VERSION 3.22)

project(ecs-benchmark)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        quadcube-engine
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "apps")
target_interface_group(common-utils)
target_interface_group(quadcube-engine)
