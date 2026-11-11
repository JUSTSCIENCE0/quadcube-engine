# Copyright (c) 2025-2026, Yakov Usoltsev
# Email: yakovmen62@gmail.com
#
# License: MIT

cmake_minimum_required(VERSION 3.22)

project(models-demo)

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

qce_import_obj_if_missing(
    ${PROJECT_NAME}
    "${CMAKE_CURRENT_LIST_DIR}/assets/prism.obj"
    "${CMAKE_CURRENT_LIST_DIR}/../common/assets/meshes/prism.qcsm"
    "${CMAKE_CURRENT_LIST_DIR}/../common/assets/materials/prism_material.qcmd"
)
qce_import_obj_if_missing(
    ${PROJECT_NAME}
    "${CMAKE_CURRENT_LIST_DIR}/assets/palm.obj"
    "${CMAKE_CURRENT_LIST_DIR}/../common/assets/meshes/palm.qcsm"
    "${CMAKE_CURRENT_LIST_DIR}/../common/assets/materials/palm_material.qcmd"
)
qce_import_obj_if_missing(
    ${PROJECT_NAME}
    "${CMAKE_CURRENT_LIST_DIR}/assets/palm_smooth.obj"
    "${CMAKE_CURRENT_LIST_DIR}/../common/assets/meshes/palm_smooth.qcsm"
)

qce_create_resources_directory(${PROJECT_NAME})
set(SHADERS_QCE
    simple_vs
    simple_ps
)
use_qce_shaders(${PROJECT_NAME} ${SHADERS_QCE})
qce_add_assets(${PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/../common/assets" FALSE) # TODO: add option to copy assets to resources directory
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    qce_copy_configs_directory(${PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/../common/configs_win")
endif() # Windows
qce_copy_scenes_directory(${PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/../common/scenes")

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "apps")
target_interface_group(common-utils)
target_interface_group(quadcube-engine)
