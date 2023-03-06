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

add_subdirectory (${FLTK_SRC_DIR})

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

set (WIN32_DEP_LIBRARIES
    pixman-1
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
