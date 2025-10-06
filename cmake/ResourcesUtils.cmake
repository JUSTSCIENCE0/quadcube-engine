# Copyright (c) 2025, Yakov Usoltsev
# Email: yakovmen62@gmail.com
#
# License: MIT

# Create resources directory
function(qce_create_resources_directory target_name)
    if (NOT TARGET ${target_name})
        message(FATAL_ERROR "Target ${target_name} does not exist")
    endif()

    add_custom_command(
        TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${target_name}>/resources/shaders/hlsl"
        COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${target_name}>/resources/shaders/glsl"
    )
endfunction()

# Copy default QCE shaders to application build directory
function(use_qce_shaders target_name)
    if (NOT TARGET ${target_name})
        message(FATAL_ERROR "Target ${target_name} does not exist")
    endif()

    set(shaders_list ${ARGN})
    set(QCE_HLSL_PREFIX ${QCE_SHADERS_BINARY_DIR}/hlsl)
    set(QCE_GLSL_PREFIX ${QCE_SHADERS_BINARY_DIR}/glsl)

    foreach(file IN LISTS shaders_list)
        # copy HLSL byte-code
        set(hlsl_path "${QCE_HLSL_PREFIX}/${file}.cso")
        if (EXISTS "${hlsl_path}")
            add_custom_command(
                TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        "${hlsl_path}"
                        "$<TARGET_FILE_DIR:${target_name}>/resources/shaders/hlsl/"
            )
        else()
            message(WARNING "HLSL shader ${file} not found, path ${hlsl_path}")
        endif()

        # copy GLSL byte-code
        set(glsl_path "${QCE_GLSL_PREFIX}/${file}.spv")
        if (EXISTS "${glsl_path}")
            add_custom_command(
                TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        "${glsl_path}"
                        "$<TARGET_FILE_DIR:${target_name}>/resources/shaders/glsl/"
            )
        else()
            message(WARNING "GLSL shader ${file} not found, path ${glsl_path}")
        endif()
    endforeach()
endfunction()
