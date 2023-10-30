# Notice about dependencies
#
# - Cairo: DirectWrite is disabled, because it causes build error here, and infamousPlugins mainly draw UI in images,
#          but rarely draw text.

# Meson is essential!
find_program (MESON_EXECUTABLE_CHECK meson REQUIRED)

if (NOT EXTERNAL_PROJECT_INCLUDED)
    include (ExternalProject)
    set (EXTERNAL_PROJECT_INCLUDED ON)
endif ()

set (CAIRO_DOWNLOAD_DIR)

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

    CONFIGURE_COMMAND  meson setup ${MESON_RECONF_FLAG} --prefix=${DEPS_PREFIX_PATH} --buildtype=plain --wrap-mode=nofallback --default-library=static -Dgtk=disabled -Da64-neon=disabled ${EXTERNAL_PROJECT_SOURCE_PATH}/pixman
    BUILD_COMMAND meson compile
    INSTALL_COMMAND meson install
)

ExternalProject_Add (
    cairo_from-source
    DOWNLOAD_DIR ${EXTERNAL_PROJECT_DOWNLOAD_PATH}
    SOURCE_DIR ${EXTERNAL_PROJECT_SOURCE_PATH}/cairo

    GIT_REPOSITORY "https://gitlab.freedesktop.org/cairo/cairo.git" GIT_SHALLOW
    GIT_COMMIT "c3b672634f"
    UPDATE_DISCONNECTED ON  # need this to avoid constant rebuild

    CONFIGURE_COMMAND  meson setup ${MESON_RECONF_FLAG} --prefix=${DEPS_PREFIX_PATH} --buildtype=plain --wrap-mode=nofallback --default-library=static -Dauto_features=enabled -Dxlib=disabled -Dxcb=disabled -Dtests=disabled -Dspectre=disabled -Dtee=enabled -Dsymbol-lookup=disabled -Ddwrite=disabled ${EXTERNAL_PROJECT_SOURCE_PATH}/cairo
    BUILD_COMMAND meson compile
    INSTALL_COMMAND meson install
)

set (CAIRO_FOUND ON)
set (CAIRO_LIBRARIES cairo pixman-1)
set (CAIRO_LIBRARY_DIRS ${DEPS_PREFIX_PATH}/lib)
set (CAIRO_INCLUDE_DIRS ${DEPS_PREFIX_PATH}/include/cairo)

# To link against static Cairo on Win32 platform, make sure you've defined macro CAIRO_WIN32_STATIC_BUILD.
# Otherwise, plenty of "undefined reference to __impl_cairo_*" linker error will occur!
# See:
#   - https://cairo.cairographics.narkive.com/NRTD7KEw/win32-static-linking
#   - https://gitlab.freedesktop.org/cairo/cairo/-/merge_requests/265
#   - https://cgit.freedesktop.org/cairo/tree/src/win32/cairo-win32-system.c
if (WIN32)
    add_compile_definitions (CAIRO_WIN32_STATIC_BUILD=1)
endif ()
