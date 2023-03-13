# Notice about dependencies
#
# - Cairo: DirectWrite is disabled, because it causes build error here, and infamousPlugins mainly draw UI in images,
#          but rarely draw text.

if (NOT EXTERNAL_PROJECT_INCLUDED)
    include (ExternalProject)
    set (EXTERNAL_PROJECT_INCLUDED ON)
endif ()

# Build type options
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set (MESON_BUILDTYPE_FLAG "--buildtype=release")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (MESON_BUILDTYPE_FLAG "--buildtype=debug")
elseif (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set (MESON_BUILDTYPE_FLAG "--buildtype=minsize")
else ()  # Plain by default
    set (MESON_BUILDTYPE_FLAG "--buildtype=plain")
endif ()

# Compiler selections
string (TOLOWER "${CMAKE_C_COMPILER}" __CMAKE_C_COMPILER__lower)
string (TOLOWER "${CMAKE_CXX_COMPILER}" __CMAKE_CXX_COMPILER__lower)

string (REGEX MATCH ".*clang.*" C_COMPILER_IS_CLANG ${__CMAKE_C_COMPILER__lower})
string (REGEX MATCH ".*clang.*" CXX_COMPILER_IS_CLANG ${__CMAKE_CXX_COMPILER__lower})

if (C_COMPILER_IS_CLANG OR CXX_COMPILER_IS_CLANG)
    set (MESON_COMPILER_ENV env CC=clang CXX=clang++)
    message (WARNING "Due to limitations of Meson, CCache (or other compiler launchers) is not supported when building Cairo!")
elseif (MSVC)
    message (FATAL_ERROR "MSVC support is not yet implemented!")
endif ()

# For debug purpose only
set (NEED_RECONFIGURE OFF)
if (NEED_RECONFIGURE)
    set (MESON_RECONF_FLAG --reconfigure)
endif ()

ExternalProject_Add (
    pixman_from-source
    DOWNLOAD_DIR ${EXTERNAL_PROJECT_DOWNLOAD_PATH}
    SOURCE_DIR ${EXTERNAL_PROJECT_SOURCE_PATH}/pixman

    URL "https://xorg.freedesktop.org/releases/individual/lib/pixman-0.42.2.tar.xz"
    #URL_SHA256 "5747d2ec498ad0f1594878cc897ef5eb6c29e91c53b899f7f71b506785fc1376"
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    UPDATE_DISCONNECTED ON

    CONFIGURE_COMMAND ${MESON_COMPILER_ENV} meson setup ${MESON_RECONF_FLAG} --prefix=${DEPS_PREFIX_PATH} ${MESON_BUILDTYPE_FLAG} --wrap-mode=nofallback --default-library=static -Dgtk=disabled -Da64-neon=disabled ${EXTERNAL_PROJECT_SOURCE_PATH}/pixman
    BUILD_COMMAND ninja
    INSTALL_COMMAND ninja install
)

ExternalProject_Add (
    cairo_from-source
    DOWNLOAD_DIR ${EXTERNAL_PROJECT_DOWNLOAD_PATH}
    SOURCE_DIR ${EXTERNAL_PROJECT_SOURCE_PATH}/cairo

    GIT_REPOSITORY "https://gitlab.freedesktop.org/cairo/cairo.git" GIT_SHALLOW
    GIT_COMMIT "c3b672634f"
    UPDATE_DISCONNECTED ON  # need this to avoid constant rebuild

    CONFIGURE_COMMAND ${MESON_COMPILER_ENV} meson setup ${MESON_RECONF_FLAG} --prefix=${DEPS_PREFIX_PATH} ${MESON_BUILDTYPE_FLAG} --wrap-mode=nofallback --default-library=static -Dauto_features=enabled -Dxlib=disabled -Dxcb=disabled -Dtests=disabled -Dspectre=disabled -Dtee=enabled -Dsymbol-lookup=disabled -Ddwrite=disabled ${EXTERNAL_PROJECT_SOURCE_PATH}/cairo
    BUILD_COMMAND ninja
    INSTALL_COMMAND ninja install
)

set (CAIRO_FOUND ON)
set (CAIRO_LIBRARIES cairo pixman-1)
set (CAIRO_LIBRARY_DIRS ${DEPS_PREFIX_PATH}/lib)
set (CAIRO_INCLUDE_DIRS ${DEPS_PREFIX_PATH}/include/cairo)
