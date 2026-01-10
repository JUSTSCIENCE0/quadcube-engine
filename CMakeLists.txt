# Copyright (c) 2025-2026, Yakov Usoltsev
# Email: yakovmen62@gmail.com
#
# License: MIT

cmake_minimum_required(VERSION 3.22)

project(quadcube-engine LANGUAGES C CXX)

if (NOT PROJECT_IS_TOP_LEVEL)
    message(STATUS "quadcube-engine: applications and tests are disabled in submodule")
    set(BUILD_QCE_APPS OFF)
    set(BUILD_QCE_TEST OFF)
endif()

option(BUILD_QCE_APPS          "Build example applications" ON)
option(BUILD_QCE_TEST          "Build tests" ON)
option(QCE_ECS_ADD_COMPONENTS  "Use additional Components set" OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(
        # warnings options
        /W4 /WX
        /wd4505 /wd4100
        /DUNICODE /D_UNICODE  # use unicode
        /MP                   # multiple processes build
        /Zc:preprocessor      # __VA_ARG__ support
    )
elseif ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    add_compile_options(
        # warnings options
        -Wall -Wextra -Wpedantic -Wcast-align -Wcast-qual
        -Wdisabled-optimization -Wformat=2 -Winit-self 
        -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual
        -Wredundant-decls -Wshadow -Wsign-promo 
        -Wundef -Wno-unused -Wno-variadic-macros
        -Wno-parentheses -Wno-unused-parameter -fdiagnostics-show-option
        -Werror
    )
endif() # GCC & Clang

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(FetchContent)
FetchContent_Declare(
    common-utils
    GIT_REPOSITORY https://github.com/JUSTSCIENCE0/common-utils.git
    GIT_TAG        dev # TODO: use commit hash when sync with main
)
if (BUILD_QCE_TEST)
    set(ENABLE_CU_TEST_UTILS ON CACHE BOOL "" FORCE)
    set(ENABLE_CU_PROFILE ON CACHE BOOL "" FORCE)
endif(BUILD_QCE_TEST)
FetchContent_MakeAvailable(common-utils)
set_property(TARGET common-utils PROPERTY FOLDER "externals")
list(APPEND CMAKE_MODULE_PATH "${common-utils_SOURCE_DIR}/cmake")

set(QCE_INTERFACE
    include/qce/qce.hpp
)

set(QCE_CONFIGS_HDRS
    include/qce/configs/camera_config.hpp
    include/qce/configs/render_config.hpp
    include/qce/configs/hid_config.hpp
    include/qce/configs/movement_config.hpp
)

set(QCE_OBJECTS_HDRS
    include/qce/objects/index.hpp
    include/qce/objects/mesh.hpp
    include/qce/objects/resource_manager.hpp
    include/qce/objects/scene.hpp
    include/qce/objects/figures.hpp
    include/qce/objects/shader.hpp
    include/qce/objects/command.hpp
)
set(QCE_OBJECTS_IMPL
    src/resource_manager.cpp
    src/figures.cpp
)

set(QCE_MATH_HDRS
    include/qce/math/vector.hpp
    include/qce/math/matrix.hpp
    include/qce/math/math.hpp
)
set(QCE_MATH_IMPL
    include/qce/math/impl/vector_def.hpp
    include/qce/math/impl/vector_sse2.hpp
    include/qce/math/impl/vector_avx2.hpp
    include/qce/math/impl/vector_avx512.hpp
    include/qce/math/impl/matrix_def.hpp
    include/qce/math/impl/matrix_sse2.hpp
    include/qce/math/impl/matrix_avx2.hpp
    include/qce/math/impl/matrix_avx512.hpp
)

set(QCE_COMPONENTS_HDRS
    include/qce/components/common.hpp
    include/qce/components/transform.hpp
    include/qce/components/movement.hpp
    include/qce/components/camera.hpp
)
set(QCE_COMPONENTS_IMPL
    src/transform.cpp
    src/camera.cpp
)

set(QCE_SYSTEMS_HDRS
    include/qce/systems/render_system.hpp
    include/qce/systems/transform_system.hpp
    include/qce/systems/movement_system.hpp
    include/qce/systems/camera_system.hpp
    include/qce/systems/hid_system.hpp
)
set(QCE_SYSTEMS_IMPL
    src/render_system.cpp
    src/camera_system.cpp
    src/movement_system.cpp
)

set(QCE_RENDERS_HDRS
    include/qce/renders/render.hpp
    include/qce/renders/windows_output.hpp
    include/qce/renders/render_type.hpp
    include/qce/renders/render_base.hpp

    # TODO: include/qce/renders/render_vulkan.hpp
)
set(QCE_RENDERS_IMPL
    src/windows_output.cpp
    src/render_base.cpp
)

set(QCE_HID_HDRS
    include/qce/hid/events.hpp
    include/qce/hid/events_handler.hpp
)

set(QCE_ECS_HDRS
    include/qce/ecs/ecs.hpp
    include/qce/ecs/entity_manager.hpp
    include/qce/ecs/component.hpp
    include/qce/ecs/query_cache.hpp
    include/qce/ecs/systems_hub.hpp
)

set(QCE_ANCILLARY_HDRS
    include/qce/ancillary/about.hpp
    include/qce/ancillary/error_codes.hpp
    include/qce/ancillary/timer.hpp
)
set(QCE_ANCILLARY_IMPL
    src/about.cpp
)

set(QCE_CMAKE_UTILS
    cmake/ResourcesUtils.cmake)

if (QCE_ECS_ADD_COMPONENTS)
    if (NOT DEFINED QCE_ECS_ADDITIONAL_COMPONENTS_HEADER OR
            "${QCE_ECS_ADDITIONAL_COMPONENTS_HEADER}" STREQUAL "")
        message(FATAL_ERROR "QCE_ECS_ADDITIONAL_COMPONENTS_HEADER is not defined while QCE_ECS_ADD_COMPONENTS is ON")
    endif() # !QCE_ECS_ADDITIONAL_COMPONENTS_HEADER
    set(QCE_ECS_HDRS
        ${QCE_ECS_HDRS}
        ${QCE_ECS_ADDITIONAL_COMPONENTS_HEADER}
    )
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            QCE_ECS_ADDITIONAL_COMPONENTS_HEADER="${QCE_ECS_ADDITIONAL_COMPONENTS_HEADER}")
endif() # QCE_USE_CUSTOM_ECS_CORE

set(QCE_SHADERS_DIR shaders)
set(QCE_HLSL_SHADERS_DIR ${QCE_SHADERS_DIR}/hlsl)
set(QCE_HLSL_VS ${QCE_HLSL_SHADERS_DIR}/simple_vs.hlsl)
set(QCE_HLSL_PS ${QCE_HLSL_SHADERS_DIR}/simple_ps.hlsl)
set(QCE_HLSL_SHADERS
    ${QCE_HLSL_VS}
    ${QCE_HLSL_PS}
)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(QCE_RENDERS_HDRS
        ${QCE_RENDERS_HDRS}
        include/qce/renders/render_dx12.hpp
        include/qce/renders/render_dx12_helpers.hpp
    )
    set(QCE_RENDERS_IMPL
        ${QCE_RENDERS_IMPL}
        src/render_dx12.cpp
    )

    set(QCE_HID_HDRS
        ${QCE_HID_HDRS}
        include/qce/hid/win_events.hpp
        include/qce/hid/win_gamepad.hpp
    )

    set(QCE_SHADERS
        ${QCE_SHADERS}
        ${QCE_HLSL_SHADERS}
    )
endif() # Windows

add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME}
    PUBLIC
        ${QCE_INTERFACE}
    PRIVATE
        ${QCE_MATH_HDRS}
        ${QCE_MATH_IMPL}

        ${QCE_RENDERS_HDRS}
        ${QCE_RENDERS_IMPL}

        ${QCE_HID_HDRS}

        ${QCE_CONFIGS_HDRS}

        ${QCE_ECS_HDRS}

        ${QCE_COMPONENTS_HDRS}
        ${QCE_COMPONENTS_IMPL}

        ${QCE_SYSTEMS_HDRS}
        ${QCE_SYSTEMS_IMPL}

        ${QCE_OBJECTS_HDRS}
        ${QCE_OBJECTS_IMPL}

        ${QCE_ANCILLARY_HDRS}
        ${QCE_ANCILLARY_IMPL}

        ${QCE_SHADERS}
)

source_group("Interface"                 FILES ${QCE_INTERFACE})

source_group("Math/Headers"              FILES ${QCE_MATH_HDRS})
source_group("Math/Implementation"       FILES ${QCE_MATH_IMPL})

source_group("Renders/Headers"           FILES ${QCE_RENDERS_HDRS})
source_group("Renders/Implementation"    FILES ${QCE_RENDERS_IMPL})

source_group("HID/Headers"               FILES ${QCE_HID_HDRS})

source_group("Configs/Headers"           FILES ${QCE_CONFIGS_HDRS})

source_group("ECS/Headers"               FILES ${QCE_ECS_HDRS})

source_group("Components/Headers"        FILES ${QCE_COMPONENTS_HDRS})
source_group("Components/Implementation" FILES ${QCE_COMPONENTS_IMPL})

source_group("Systems/Headers"           FILES ${QCE_SYSTEMS_HDRS})
source_group("Systems/Implementation"    FILES ${QCE_SYSTEMS_IMPL})

source_group("Objects/Headers"           FILES ${QCE_OBJECTS_HDRS})
source_group("Objects/Implementation"    FILES ${QCE_OBJECTS_IMPL})

source_group("Ancillary/Headers"         FILES ${QCE_ANCILLARY_HDRS})
source_group("Ancillary/Implementation"  FILES ${QCE_ANCILLARY_IMPL})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    source_group("Shaders/HLSL" FILES ${QCE_HLSL_SHADERS})
endif() # Windows

set_source_files_properties(${QCE_SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE)
set(QCE_SHADERS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

# add HLSL shaders compilation
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(QCE_HLSL_BINARY_FILENAME "${QCE_SHADERS_BINARY_DIR}/hlsl/%(Filename).cso")
        set(QCE_HLSL_SHADER_MODEL 5.1) # TODO: make it option?
        set_source_files_properties(${QCE_HLSL_VS} PROPERTIES
            HEADER_FILE_ONLY FALSE
            VS_SHADER_TYPE Vertex
            VS_SHADER_MODEL ${QCE_HLSL_SHADER_MODEL}
            VS_SHADER_OBJECT_FILE_NAME ${QCE_HLSL_BINARY_FILENAME}
        )
        set_source_files_properties(${QCE_HLSL_PS} PROPERTIES
            HEADER_FILE_ONLY FALSE
            VS_SHADER_TYPE Pixel
            VS_SHADER_MODEL ${QCE_HLSL_SHADER_MODEL}
            VS_SHADER_OBJECT_FILE_NAME ${QCE_HLSL_BINARY_FILENAME}
        )
    else()
        message(WARNING "HLSL shader auto-compilation implemented only for MSVC")
    endif() # MSVC
endif() # Windows

target_include_directories(${PROJECT_NAME}
    PUBLIC
        include/
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        common-utils
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    FetchContent_Declare(
        DirectX-Headers
        GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers.git
        GIT_TAG        c94b9b23aaadc2034dd1cad656a5a69f1526f98a # v1.616.0
    )
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(DirectX-Headers)

    set_property(TARGET DirectX-Headers DirectX-Guids PROPERTY FOLDER "externals")

    target_link_libraries(${PROJECT_NAME}
        PUBLIC
            DirectX-Headers
            DirectX-Guids

            D3D12.lib
            DXGI.lib

            xinput.lib
    )
endif()

# QCE CMake utils
include(${QCE_CMAKE_UTILS})

set_property(TARGET ${PROJECT_NAME} PROPERTY INCLUDE_DIR_POSTFIX "qce")

if (BUILD_QCE_APPS)
    add_subdirectory(apps)
endif(BUILD_QCE_APPS)

if (BUILD_QCE_TEST)
    add_subdirectory(tests)
endif(BUILD_QCE_TEST)

target_interface_group(common-utils)
