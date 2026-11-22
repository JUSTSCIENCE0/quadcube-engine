# Copyright (c) 2026, Yakov Usoltsev
# Email: yakovmen62@gmail.com
#
# License: MIT

include(ExternalProject)

set(STB_SOURCE_DIR "${CMAKE_BINARY_DIR}/externals/stb")

ExternalProject_Add(stb_external
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        6b5799bae8a1fe3792691ef4b72ae26434c3e799 # master 22.11.26
    GIT_SHALLOW    TRUE

    SOURCE_DIR "${STB_SOURCE_DIR}"

    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ""

    UPDATE_DISCONNECTED TRUE
)

add_library(stb INTERFACE)

target_include_directories(stb INTERFACE
    "${STB_SOURCE_DIR}"
)

add_dependencies(stb stb_external)
