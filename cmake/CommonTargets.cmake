# CommonTargets.cmake
# Common target configurations and helper functions

# Helper function to create a library with consistent settings
function(add_robotics_library)
    set(options HEADER_ONLY BARE_METAL)
    set(oneValueArgs NAME NAMESPACE)
    set(multiValueArgs SOURCES PUBLIC_HEADERS PRIVATE_HEADERS DEPENDENCIES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(ARG_HEADER_ONLY)
        # Header-only library
        add_library(${ARG_NAME} INTERFACE)
        target_include_directories(${ARG_NAME} INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        )
        if(ARG_PUBLIC_HEADERS)
            target_sources(${ARG_NAME} INTERFACE
                FILE_SET HEADERS
                BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
                FILES ${ARG_PUBLIC_HEADERS}
            )
        endif()
    else()
        # Compiled library
        add_library(${ARG_NAME} ${ARG_SOURCES})
        target_include_directories(${ARG_NAME}
            PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:include>
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
        apply_common_settings(${ARG_NAME})

        if(ARG_BARE_METAL)
            configure_bare_metal(${ARG_NAME})
        endif()
    endif()

    # Add namespace alias
    if(ARG_NAMESPACE)
        add_library(${ARG_NAMESPACE}::${ARG_NAME} ALIAS ${ARG_NAME})
    endif()

    # Link dependencies
    if(ARG_DEPENDENCIES)
        target_link_libraries(${ARG_NAME} PUBLIC ${ARG_DEPENDENCIES})
    endif()

    # Installation
    install(TARGETS ${ARG_NAME}
        EXPORT RoboticsLibsTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    if(ARG_PUBLIC_HEADERS)
        install(FILES ${ARG_PUBLIC_HEADERS}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${ARG_NAME}
        )
    endif()
endfunction()
