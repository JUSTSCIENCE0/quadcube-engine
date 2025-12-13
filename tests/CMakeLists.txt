# Copyright (c) 2025, Yakov Usoltsev
# Email: yakovmen62@gmail.com
#
# License: MIT

cmake_minimum_required(VERSION 3.22)

option(USE_TINY_DATASET "Use reduced dataset instead of regular one" OFF)

if (USE_TINY_DATASET)
    set(QCE_TEST_DATA_URL https://www.kaggle.com/api/v1/datasets/download/adaadaaa/qce-test-data-tiny)
    set(QCE_TEST_DATA_PATH ${CMAKE_CURRENT_LIST_DIR}/test_data_tiny)
    set(QCE_TEST_DATA_ZIP_FILE ${CMAKE_CURRENT_LIST_DIR}/test_data_tiny.zip)
else(USE_TINY_DATASET)
    set(QCE_TEST_DATA_URL https://www.kaggle.com/api/v1/datasets/download/adaadaaa/qce-test-data-full)
    set(QCE_TEST_DATA_PATH ${CMAKE_CURRENT_LIST_DIR}/test_data_full)
    set(QCE_TEST_DATA_ZIP_FILE ${CMAKE_CURRENT_LIST_DIR}/test_data_full.zip)
endif(USE_TINY_DATASET)

if (NOT EXISTS ${QCE_TEST_DATA_PATH})
    message(STATUS "Downloading test data from ${QCE_TEST_DATA_URL}")
    file(DOWNLOAD "${QCE_TEST_DATA_URL}" "${QCE_TEST_DATA_ZIP_FILE}" SHOW_PROGRESS)

    file(MAKE_DIRECTORY "${QCE_TEST_DATA_PATH}")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${QCE_TEST_DATA_ZIP_FILE}"
        WORKING_DIRECTORY "${QCE_TEST_DATA_PATH}"
    )

    file(REMOVE "${QCE_TEST_DATA_ZIP_FILE}")
endif()

add_subdirectory(math-test)
add_subdirectory(ecs-test)
