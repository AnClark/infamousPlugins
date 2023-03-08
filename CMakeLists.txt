cmake_minimum_required(VERSION 2.8)

project(infamous-plugins)

option(MOD "Install MOD gui resources" OFF)

option(CLASSIC "Build the original LV2 plugins" OFF)
option(DPF "Build DPF ported plugins" ON)

set(LIBDIR lib CACHE STRING "Specifies the name of the library path")

set( PLUGINS_CLASSIC "casynth;envfollower;hip2b;powerup;powercut;cheapdist;stuck;ewham;lushlife;bentdelay;mindi;octolo" CACHE STRING "List of plugins to build (classic LV2 edition)")
#set( PLUGINS_DPF "casynth;envfollower;hip2b;powerup;powercut;cheapdist;stuck;ewham;lushlife;bentdelay;mindi;octolo" CACHE STRING "List of plugins to build (DPF ported edition)")
set( PLUGINS_DPF "casynth" CACHE STRING "List of plugins to build (DPF ported edition)")

set(EXTERNAL_PROJECT_DOWNLOAD_PATH ${PROJECT_BINARY_DIR}/deps/download)
set(EXTERNAL_PROJECT_SOURCE_PATH ${PROJECT_BINARY_DIR}/deps/source)
set(DEPS_PREFIX_PATH ${PROJECT_BINARY_DIR}/usr)

option(USE_SYSTEM_CAIRO "Use system-provided Cairo library. Say NO if you want to build from source (for compatibility or debug)" ON)
if (NOT USE_SYSTEM_CAIRO)
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")  # Append more than once does not matter
    include(BuildCairo)
endif ()

# Include FLTK support on non-X11 environments
if (WIN32 OR APPLE)
    # Now FLTK supports Cairo driver
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
    include(BuildFLTK)
endif ()

if (CLASSIC)
    foreach( plug ${PLUGINS_CLASSIC} )
        add_subdirectory(src/${plug})
    endforeach(plug)
endif ()

if (DPF)
    add_subdirectory (vendor/dpf)

    foreach( plug ${PLUGINS_DPF} )
        add_subdirectory(plugins/${plug})
    endforeach(plug)
endif ()

add_executable(infamous-rule
    src/rule.c
)
install(TARGETS infamous-rule
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

# uninstall target
# NOTICE: only add it on Linux, since FLTK has the same statements
if(NOT WIN32)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
