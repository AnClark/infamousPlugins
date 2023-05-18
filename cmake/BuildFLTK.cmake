# Set a bundle of FLTK build options
set (FLTK_BUILD_EXAMPLES OFF CACHE BOOL "")
set (FLTK_BUILD_TEST OFF CACHE BOOL "")
set (FLTK_BUILD_FLTK_OPTIONS OFF CACHE BOOL "")
set (FLTK_BUILD_FLUID ON CACHE BOOL "")

set (FLTK_SRC_DIR ${PROJECT_SOURCE_DIR}/vendor/fltk)

set (OPTION_BUILD_HTML_DOCUMENTATION OFF CACHE BOOL "")
set (OPTION_BUILD_SHARED_LIBS OFF CACHE BOOL "")

set (OPTION_CAIRO ON CACHE BOOL "")
set (OPTION_CAIROEXT ON CACHE BOOL "")
set (OPTION_USE_GDIPLUS ON CACHE BOOL "")
set (OPTION_USE_SVG ON CACHE BOOL "")

set (OPTION_FILESYSTEM_SUPPORT ON CACHE BOOL "")

set (OPTION_USE_SYSTEM_LIBJPEG OFF CACHE BOOL "")
set (OPTION_USE_SYSTEM_LIBPNG OFF CACHE BOOL "")
set (OPTION_USE_SYSTEM_ZLIB OFF CACHE BOOL "")

# AnClark's hack into FLTK: enable embed window related API
set (OPTION_EMBED_WINDOW ON CACHE BOOL "")

# -- End of FLTK build options

add_subdirectory (${FLTK_SRC_DIR})

if (NOT USE_SYSTEM_CAIRO)
    include_directories (
        ${CAIRO_INCLUDE_DIRS}    
        ${DEPS_PREFIX_PATH}/include
    )

    # Cairo must be built before FLTK
    add_dependencies(fltk cairo_from-source)
    add_dependencies(fltk_cairo cairo_from-source)
endif ()

# For compatibility, assign NTK-related vars to FLTK
set (NTK_FOUND ON)
set (NTK_INCLUDE_DIRS ${FLTK_SRC_DIR} ${PROJECT_BINARY_DIR}/vendor/fltk)
set (NTK_LIBRARY_DIRS "")   # CMake target does not need this
set (NTK_LIBRARIES
    fltk
    fltk_cairo
    #fltk_jpeg
    #fltk_images
    fltk_z
)
set (FLUID_EXECUTABLE_NAME fluid)

# Pixman is already included in CAIRO_LIBRARIES when build from source
if (USE_SYSTEM_CAIRO)
    set (LINK_pixman pixman-1)
endif ()

set (WIN32_DEP_LIBRARIES
    ${LINK_pixman}
    fontconfig
    freetype
    intl            # required by fontconfig
    iconv           # required by fontconfig
    bz2             # required by freetype
    brotlidec       # required by freetype
    brotlicommon    # required by brotlidec
    harfbuzz        # required by freetype
    graphite2       # required by harfbuzz
    png16           # required by fontconfig
    z               # required by libpng
    expat           # required by fontconfig
    Msimg32         # for __imp_GradientFill
    Rpcrt4          # for __imp_UuidCreate
)
