# Copyright (c) 2026, Yakov Usoltsev
# Email: yakovmen62@gmail.com
#
# License: MIT

find_path(Compressonator_INCLUDE_DIR
    NAMES compressonator.h
    HINTS
        "$ENV{COMPRESSONATOR_ROOT}"
        "$ENV{AMD_COMPRESSONATOR_ROOT}"
    PATH_SUFFIXES
        include
)

find_library(Compressonator_LIBRARY_RELEASE
    NAMES Compressonator_MD
    HINTS
        "$ENV{COMPRESSONATOR_ROOT}"
        "$ENV{AMD_COMPRESSONATOR_ROOT}"
    PATH_SUFFIXES
        lib/bin/x64
)

find_library(Compressonator_LIBRARY_DEBUG
    NAMES Compressonator_MDd
    HINTS
        "$ENV{COMPRESSONATOR_ROOT}"
        "$ENV{AMD_COMPRESSONATOR_ROOT}"
    PATH_SUFFIXES
        lib/bin/x64
)

if(Compressonator_INCLUDE_DIR)
    message(STATUS "Compressonator headers found: ${Compressonator_INCLUDE_DIR}")
else()
    message(STATUS "Compressonator headers not found")
endif()

if(Compressonator_LIBRARY_RELEASE)
    message(STATUS "Compressonator Release library found: ${Compressonator_LIBRARY_RELEASE}")
else()
    message(STATUS "Compressonator Release library not found")
endif()

if(Compressonator_LIBRARY_DEBUG)
    message(STATUS "Compressonator Debug library found: ${Compressonator_LIBRARY_DEBUG}")
else()
    message(STATUS "Compressonator Debug library not found")
endif()

if(Compressonator_INCLUDE_DIR AND Compressonator_LIBRARY_RELEASE AND Compressonator_LIBRARY_DEBUG)
    message(STATUS "Compressonator SDK found")
else()
    message(STATUS "Compressonator SDK not found")
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Compressonator
    REQUIRED_VARS
        Compressonator_INCLUDE_DIR
        Compressonator_LIBRARY_RELEASE
        Compressonator_LIBRARY_DEBUG
)

if(Compressonator_FOUND AND NOT TARGET Compressonator)
    add_library(Compressonator STATIC IMPORTED)

    set_target_properties(Compressonator PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Compressonator_INCLUDE_DIR}"

        IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"

        IMPORTED_LOCATION_DEBUG "${Compressonator_LIBRARY_DEBUG}"
        IMPORTED_LOCATION_RELEASE "${Compressonator_LIBRARY_RELEASE}"

        MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE
        MAP_IMPORTED_CONFIG_MINSIZEREL RELEASE
    )

    if(MSVC)
        target_link_options(Compressonator INTERFACE
            "$<$<CONFIG:Debug>:/IGNORE:4099>"
        )
    endif()
endif()

mark_as_advanced(
    Compressonator_INCLUDE_DIR
    Compressonator_LIBRARY_RELEASE
    Compressonator_LIBRARY_DEBUG
)