set (PLUGIN casynth_dpf)

#
# Compiler optimization flags
#

include (CheckCCompilerFlag)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g -std=c99 -Og")
check_c_compiler_flag("-msse2 -mfpmath=sse" SUPPORTS_SSE )
check_c_compiler_flag("-ffast-math" SUPPORTS_FAST_MATH )
if (SUPPORTS_SSE)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse2 -mfpmath=sse")
endif ()
if (SUPPORTS_FAST_MATH)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffast-math")
endif () 
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g") 

#
# UI dependency stuff
#

# check for our various libraries
find_package (PkgConfig)
if (USE_SYSTEM_CAIRO)
    message(STATUS "Using cairo provided by system.")
    pkg_check_modules (CAIRO cairo)
else ()
    message(STATUS "Using cairo built from source.")
endif ()
if (WIN32 OR APPLE)
    message(STATUS "Running on non-X11 environments. Using FLTK.")
else ()
    # On X11, prefer NTK
    pkg_check_modules (NTK ntk)

    set (FLUID_EXECUTABLE_NAME ntk-fluid)
endif ()


include_directories (
    ${NTK_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src/ffffltk
    ${PROJECT_SOURCE_DIR}/src/draw
    .
)

link_directories (${NTK_LIBRARY_DIRS} ${CAIRO_LIBRARY_DIRS})

#
# UI stuff
#

if (CAIRO_FOUND AND NTK_FOUND)
    find_program (FLTK_FLUID_EXECUTABLE ${FLUID_EXECUTABLE_NAME})

    # fltk_wrap_ui() will pre-define a target.
    # Set target to DPF UI. This can avoid warnings.
    fltk_wrap_ui(${PLUGIN}-ui casynth_ui.fl)

    set (BUILD_UI ON)
else()
    message(WARNING "UI Libraries Missing, no GUI will be installed")
    set (BUILD_UI OFF)
endif()

# Do not build JACK standalone application on Windows
if (NOT WIN32)
    set (TARGET_JACK jack)
endif()

#
# Build plugin
#

if (BUILD_UI)
    dpf_add_plugin (${PLUGIN}
        TARGETS vst2 vst3 lv2 clap ${TARGET_JACK}
        UI_TYPE external
        FILES_DSP
            casynth.cpp
            engine.cpp
            note.cpp
            waves.cpp
        FILES_UI
            casynth_editor.cpp
            ${${PLUGIN}-ui_FLTK_UI_SRCS}
        FILES_COMMON
            params.cpp
    )

    target_link_libraries(${PLUGIN}-ui PRIVATE
        ${NTK_LIBRARIES}
        ${CAIRO_LIBRARIES}
        ${WIN32_DEP_LIBRARIES}
    )

    # Include FLUID-generated headers
    target_include_directories (${PLUGIN}-ui PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
    )

    if (WIN32)
        # Enable access to my custom FLTK API: Fl_Window::override_winid()
        target_compile_definitions (${PLUGIN}-ui PRIVATE USE_EMBED_WINDOW=1)
    endif ()
else ()
    dpf_add_plugin (${PLUGIN}
        TARGETS vst2 vst3 lv2 clap
        FILES_DSP
            casynth.cpp
            engine.cpp
            note.cpp
            waves.cpp
        FILES_COMMON
            params.cpp
    )
endif ()

target_include_directories (${PLUGIN} PUBLIC .)

# Linker diagnostic options
link_libraries (-Wl,--no-undefined,--unresolved-symbols=report-all)
