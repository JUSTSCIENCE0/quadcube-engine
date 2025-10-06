# Copyright (c) 2025, Yakov Usoltsev
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

option(BUILD_QCE_APPS        "Build example applications" ON)
option(BUILD_QCE_TEST        "Build tests" ON)

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
    GIT_TAG        dev # TODO: use commit hash when it will be ready
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
    include/qce/vector.hpp
    include/qce/matrix.hpp
    include/qce/math.hpp
    include/qce/render.hpp
)

set(QCE_OBJECTS
    include/qce/objects/index.hpp
    include/qce/objects/mesh.hpp
    include/qce/objects/model.hpp
    include/qce/objects/entity.hpp
    include/qce/objects/resource_manager.hpp
    include/qce/objects/scene.hpp
    include/qce/objects/figures.hpp
    include/qce/objects/shader.hpp
)

set(QCE_MATH
    include/qce/math/vector_def.hpp
    include/qce/math/vector_sse2.hpp
    include/qce/math/vector_avx2.hpp
    include/qce/math/vector_avx512.hpp
    include/qce/math/matrix_def.hpp
    include/qce/math/matrix_sse2.hpp
    include/qce/math/matrix_avx2.hpp
    include/qce/math/matrix_avx512.hpp
)

set(QCE_RENDERS
    include/qce/renders/windows_output.hpp
    include/qce/renders/render_type.hpp
    include/qce/renders/render_base.hpp
    include/qce/renders/render_dx12.hpp
    include/qce/renders/render_dx12_helpers.hpp
    # TODO: include/qce/renders/render_vulkan.hpp
)

set(QCE_ANCILLARY
    include/qce/ancillary/about.hpp
    include/qce/ancillary/error_codes.hpp
)

set(QCE_IMPLEMENTATION
    # objects
    src/resource_manager.cpp
    src/scene.cpp
    src/figures.cpp

    # renders
    src/windows_output.cpp
    src/render_base.cpp
    src/render_dx12.cpp

    #other
    src/about.cpp
)

set(QCE_SHADERS_DIR shaders)
set(QCE_HLSL_SHADERS_DIR ${QCE_SHADERS_DIR}/hlsl)
set(QCE_HLSL_VS ${QCE_HLSL_SHADERS_DIR}/simple_vs.hlsl)
set(QCE_HLSL_PS ${QCE_HLSL_SHADERS_DIR}/simple_ps.hlsl)
set(QCE_HLSL_SHADERS
    ${QCE_HLSL_VS}
    ${QCE_HLSL_PS}
)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(QCE_SHADERS
        ${QCE_SHADERS}
        ${QCE_HLSL_SHADERS}
    )
endif() # Windows

add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME}
    PUBLIC
        ${QCE_INTERFACE}
        ${QCE_OBJECTS}
        ${QCE_ANCILLARY}
    PRIVATE
        ${QCE_MATH}
        ${QCE_RENDERS}
        ${QCE_IMPLEMENTATION}
        ${QCE_SHADERS}
)

source_group("Interface" FILES ${QCE_INTERFACE})
source_group("Math" FILES ${QCE_MATH})
source_group("Objects" FILES ${QCE_OBJECTS})
source_group("Renders" FILES ${QCE_RENDERS})
source_group("Ancillary" FILES ${QCE_ANCILLARY})
source_group("Implementation" FILES ${QCE_IMPLEMENTATION})
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    source_group("Shaders/HLSL" FILES ${QCE_HLSL_SHADERS})
endif() # Windows

set_source_files_properties(${QCE_SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE)

# add HLSL shaders compilation
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(QCE_HLSL_BINARY_FILENAME "${CMAKE_CURRENT_BINARY_DIR}/shaders/hlsl/%(Filename).cso")
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
    )
endif()

# Utils
# Copy default QCE shaders to application build directory
function(use_qce_shaders target_name directory_suffix)
    if (NOT TARGET ${target_name})
        message(FATAL_ERROR "Target ${target_name} does not exist")
    endif()

    add_custom_command(
        TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${target_name}>/${directory_suffix}/hlsl"
        COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${target_name}>/${directory_suffix}/glsl"
    )

    set(shaders_list ${ARGN})
    set(QCE_HLSL_PREFIX ${QCE_SHADERS_DIR}/hlsl)
    set(QCE_GLSL_PREFIX ${QCE_SHADERS_DIR}/glsl)

    foreach(file IN LISTS shaders_list)
        # copy HLSL
        set(hlsl_path "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${QCE_HLSL_PREFIX}/${file}.hlsl")
        if (EXISTS "${hlsl_path}")
            add_custom_command(
                TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        "${hlsl_path}"
                        "$<TARGET_FILE_DIR:${target_name}>/${directory_suffix}/hlsl/"
            )
        else()
            message(WARNING "HLSL shader ${file} not found, path ${hlsl_path}")
        endif()

        # copy GLSL
        set(glsl_path "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${QCE_GLSL_PREFIX}/${file}.glsl")
        if (EXISTS "${glsl_path}")
            add_custom_command(
                TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        "${glsl_path}"
                        "$<TARGET_FILE_DIR:${target_name}>/${directory_suffix}/glsl/"
            )
        else()
            message(WARNING "GLSL shader ${file} not found, path ${hlsl_path}")
        endif()
    endforeach()
endfunction()

set_property(TARGET ${PROJECT_NAME} PROPERTY INCLUDE_DIR_POSTFIX "qce")

if (BUILD_QCE_APPS)
    add_subdirectory(apps)
endif(BUILD_QCE_APPS)

if (BUILD_QCE_TEST)
    add_subdirectory(tests)
endif(BUILD_QCE_TEST)

target_interface_group(common-utils)
