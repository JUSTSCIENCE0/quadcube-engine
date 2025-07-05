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
    GIT_TAG        main # TODO: use commit hash when it will be ready
)
if (BUILD_QCE_TEST)
    set(ENABLE_CU_TEST_UTILS ON CACHE BOOL "" FORCE)
    set(ENABLE_CU_PROFILE ON CACHE BOOL "" FORCE)
endif(BUILD_QCE_TEST)
FetchContent_MakeAvailable(common-utils)
set_property(TARGET common-utils PROPERTY FOLDER "externals")
list(APPEND CMAKE_MODULE_PATH "${common-utils_SOURCE_DIR}/cmake")

set(QCE_INTERFACE
    include/qce/about.hpp
    include/qce/common.hpp
    include/qce/vector.hpp
    include/qce/matrix.hpp
    include/qce/math.hpp
)

set(QCE_MATH
    include/qce/math/vector_def.hpp
    include/qce/math/vector_sse2.hpp
    include/qce/math/vector_avx512.hpp
    include/qce/math/matrix_def.hpp
    include/qce/math/matrix_sse2.hpp
    include/qce/math/matrix_avx512.hpp
)

set(QCE_OTHER
    src/about.cpp
)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME}
    PUBLIC
        ${QCE_INTERFACE}
    PRIVATE
        ${QCE_MATH}
        ${QCE_OTHER}
)

source_group("Interface" FILES ${QCE_INTERFACE})
source_group("Math" FILES ${QCE_MATH})
source_group("Other" FILES ${QCE_OTHER})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        include/
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        common-utils
)

set_property(TARGET ${PROJECT_NAME} PROPERTY INCLUDE_DIR_POSTFIX "qce")

if (BUILD_QCE_APPS)
    add_subdirectory(apps)
endif(BUILD_QCE_APPS)

if (BUILD_QCE_TEST)
    add_subdirectory(tests)
endif(BUILD_QCE_TEST)

target_interface_group(common-utils)
