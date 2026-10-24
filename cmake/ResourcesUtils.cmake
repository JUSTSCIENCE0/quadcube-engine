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
    foreach(file IN LISTS shaders_list)
        set(hlsl_source "${QCE_HLSL_SHADERS_DIR}/${file}.hlsl")
        set(glsl_source "${QCE_GLSL_SHADERS_DIR}/${file}.glsl")
        set(shader_source_found FALSE)

        if (EXISTS "${hlsl_source}")
            set(shader_source_found TRUE)
            if (CMAKE_SYSTEM_NAME STREQUAL "Windows"
                    AND CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
                add_custom_command(
                    TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            "${QCE_SHADERS_BINARY_DIR}/hlsl/${file}.cso"
                            "$<TARGET_FILE_DIR:${target_name}>/resources/shaders/hlsl/${file}.cso"
                    VERBATIM
                )
            endif()
        endif()

        if (EXISTS "${glsl_source}")
            set(shader_source_found TRUE)
            add_custom_command(
                TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${QCE_SHADERS_BINARY_DIR}/glsl/${file}.spv"
                        "$<TARGET_FILE_DIR:${target_name}>/resources/shaders/glsl/${file}.spv"
                VERBATIM
            )
        endif()

        if (NOT shader_source_found)
            message(WARNING
                "Shader source ${file} not found; checked "
                "${hlsl_source} and ${glsl_source}"
            )
        endif()
    endforeach()
endfunction()

# Copy assets to the target build directory
function(qce_add_assets target_name assets_dir is_copy)
    if (NOT TARGET ${target_name})
        message(FATAL_ERROR "Target ${target_name} does not exist")
    endif()

    if (NOT IS_DIRECTORY "${assets_dir}")
        message(FATAL_ERROR "Assets directory ${assets_dir} does not exist")
    endif()

    if (is_copy)
        add_custom_command(
            TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${assets_dir}"
                    "$<TARGET_FILE_DIR:${target_name}>/resources/assets/"
        )
    else()
        add_custom_command(
            TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                    "${assets_dir}"
                    "$<TARGET_FILE_DIR:${target_name}>/resources/assets/"
        )
    endif()
endfunction()
