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

# Import an OBJ file when its expected mesh or material output is missing.
function(qce_import_obj_if_missing target_name obj_file mesh_file)
    if (NOT TARGET ${target_name})
        message(FATAL_ERROR "Target ${target_name} does not exist")
    endif()
    if (NOT TARGET qce-importer)
        message(FATAL_ERROR "Target qce-importer does not exist")
    endif()
    if (NOT EXISTS "${obj_file}")
        message(FATAL_ERROR "OBJ file ${obj_file} does not exist")
    endif()

    add_dependencies(${target_name} qce-importer)

    get_filename_component(mesh_file "${mesh_file}" ABSOLUTE)
    set(import_outputs)
    set(import_directories)
    set(importer_arguments
        --input "${obj_file}"
        --conversion to_bin
    )
    if (NOT EXISTS "${mesh_file}")
        get_filename_component(mesh_directory "${mesh_file}" DIRECTORY)
        list(APPEND import_outputs "${mesh_file}")
        list(APPEND import_directories "${mesh_directory}")
        list(APPEND importer_arguments --mesh-output "${mesh_directory}")
    endif()

    if (ARGC GREATER 3 AND NOT "${ARGV3}" STREQUAL "")
        get_filename_component(material_file "${ARGV3}" ABSOLUTE)
        if (NOT EXISTS "${material_file}")
            get_filename_component(material_directory "${material_file}" DIRECTORY)
            list(APPEND import_outputs "${material_file}")
            list(APPEND import_directories "${material_directory}")
            list(APPEND importer_arguments --material-output "${material_directory}")
        endif()
    endif()

    if (import_outputs)
        get_filename_component(mesh_target_name "${mesh_file}" NAME_WE)
        set(import_target_name "${target_name}-${mesh_target_name}-import")

        add_custom_command(
            OUTPUT ${import_outputs}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${import_directories}
            COMMAND $<TARGET_FILE:qce-importer> ${importer_arguments}
            COMMENT "Importing ${obj_file}"
            VERBATIM
        )
        add_custom_target(${import_target_name} DEPENDS ${import_outputs})
        add_dependencies(${import_target_name} qce-importer)
        add_dependencies(${target_name} ${import_target_name})
    endif()
endfunction()
