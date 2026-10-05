# Copyright (c) 2026, Yakov Usoltsev
# Email: yakovmen62@gmail.com
#
# License: MIT

function(qce_copy_scenes_directory target_name scenes_directory)
    if (NOT TARGET ${target_name})
        message(FATAL_ERROR "Target ${target_name} does not exist")
    endif()

    add_custom_command(
        TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${scenes_directory}" "$<TARGET_FILE_DIR:${target_name}>/scenes"
    )
endfunction()
