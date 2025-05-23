# Copyright (c) 2025, Yakov Usoltsev
# Email: yakovmen62@gmail.com
#
# License: MIT

cmake_minimum_required(VERSION 3.22)

if (NOT TARGET googletest)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723 # v1.17.0
    )
    FetchContent_MakeAvailable(googletest)
    set_property(TARGET gmock gmock_main gtest gtest_main PROPERTY FOLDER "tests/externals")
endif() # googletest

set(QCE_TEST_DATA_PATH ${CMAKE_CURRENT_LIST_DIR}/test_data)

add_subdirectory(math-test)
