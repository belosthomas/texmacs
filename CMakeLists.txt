
###############################################################################
#
# MODULE      : CMakeLists.txt
# DESCRIPTION : CMake file for TeXmacs
# COPYRIGHT   : (C) 2008-2009  Massimiliano Gubinelli
#               (C) 2017-2020  Darcy Shen
#               (C) 2023       Liza Belos
#
# This software falls under the GNU general public license version 3 or later.
# It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
# in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.


# The name of our project is "TEXMACS". CMakeLists files in this project can
# refer to the root source directory of the project as ${TEXMACS_SOURCE_DIR} and
# to the root binary directory of the project as ${TEXMACS_BINARY_DIR}.


cmake_minimum_required(VERSION 3.13)
project(TeXmacs
        VERSION 2.1.2
        DESCRIPTION "TeXmacs is a scientific text editor"
        HOMEPAGE_URL "https://www.texmacs.org"
        LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set (VERSION_MAJOR "2")
set (VERSION_MINOR "1")
set (VERSION_BUILD "2")
set (TEXMACS_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD})

set (DEVEL_VERSION ${TEXMACS_VERSION})
set (DEVEL_RELEASE "1")
set (STABLE_VERSION ${TEXMACS_VERSION})
set (STABLE_RELEASE "1")
set (TEXMACS_REVISION ${TEXMACS_VERSION})

add_definitions(-DWIN32_LEAN_AND_MEAN)

find_package(Qt6 COMPONENTS Core Gui Widgets PrintSupport Svg OpenGL QuickWidgets WebView)
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport Svg OpenGL QuickWidgets WebView)
    if (NOT Qt5_FOUND)
        message(FATAL_ERROR "Qt5 or Qt6 is required to build TeXmacs")
    endif()
endif()

if (Qt6_FOUND)
    set(USE_GUILE_QT6 ON CACHE BOOL "Use Guile with Qt" FORCE)
endif()

if (Qt5_FOUND)
    set(USE_GUILE_QT5 ON CACHE BOOL "Use Guile with Qt" FORCE)
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

# Detect MinGW
if (MINGW OR WIN32)
     set (OS_MINGW 1)
endif()

# add a global variable to C++ code
add_definitions(-DQTTEXMACS=1)

set(PACKAGE TeXmacs)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

function(GenerateResourcesQRC)

    # Generate resources.qrc
    file(GLOB_RECURSE Texmacs_Resources "${CMAKE_CURRENT_SOURCE_DIR}/TeXmacs/*")

    set(Texmacs_Resources_List "")

    # For each file
    foreach (Texmacs_Resource ${Texmacs_Resources})
        # Get the file name
        get_filename_component(Texmacs_Resource_Name ${Texmacs_Resource} NAME)
        # Get the file path
        get_filename_component(Texmacs_Resource_Path ${Texmacs_Resource} PATH)
        # Remove the path prefix
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" Texmacs_Resource_Path ${Texmacs_Resource_Path})
        # Add the file to the resources
        # list(APPEND Texmacs_Resources_List "<file alias=\"${Texmacs_Resource_Path}/${Texmacs_Resource_Name}\">${Texmacs_Resource}</file>")
        set(Texmacs_Resources_List "${Texmacs_Resources_List}<file alias=\"${Texmacs_Resource_Path}/${Texmacs_Resource_Name}\">${Texmacs_Resource}</file>\n")
    endforeach ()

    # Generate the qrc file
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc.in" "${CMAKE_CURRENT_BINARY_DIR}/resources.qrc" @ONLY)

    # add the qrc to the texmacs target
    target_sources(texmacs PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/resources.qrc")

endfunction()

file(GLOB_RECURSE Texmacs_AppSources "${CMAKE_CURRENT_SOURCE_DIR}/src/Texmacs/Texmacs/*")

if (ANDROID)
    add_library (texmacs MODULE ${Texmacs_AppSources})
    set_target_properties(texmacs PROPERTIES QT_ANDROID_PACKAGE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/android")
    qt_android_generate_deployment_settings(texmacs)
    qt_android_add_apk_target(texmacs)

    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/android-build/libs/${ANDROID_ABI}/")
    add_custom_command(
            TARGET texmacs
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy "${CMAKE_CURRENT_BINARY_DIR}/libtexmacs.so" "${CMAKE_CURRENT_BINARY_DIR}/android-build/libs/${ANDROID_ABI}/libtexmacs_${ANDROID_ABI}.so"
    )
else()
    add_executable (texmacs)
    target_sources(texmacs PUBLIC ${Texmacs_AppSources})
    set_target_properties(texmacs PROPERTIES
        MACOSX_BUNDLE ON
        WIN32_EXECUTABLE ON
    )
    # enable QDebug
    target_compile_definitions(texmacs PUBLIC QT_MESSAGELOGCONTEXT)
endif()

# if MSVC, use /Ox in release mode
if (MSVC)
    if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        target_compile_options(texmacs PUBLIC /Ox)
    endif()
else()
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(texmacs PUBLIC -O2)
    endif()
endif()

GenerateResourcesQRC()
target_include_directories(texmacs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(WIN32)
    target_link_libraries(texmacs PUBLIC wsock32 ws2_32)
endif()

target_link_libraries(texmacs PUBLIC Qt::Gui Qt::Widgets Qt::PrintSupport Qt::Svg Qt::OpenGL Qt::QuickWidgets Qt::WebView)


#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#    find_program(TOOL_WINDEPLOYQT NAMES windeployqt.debug.bat)
#else()
#    find_program(TOOL_WINDEPLOYQT NAMES windeployqt)
#endif()

#add_custom_command(TARGET texmacs POST_BUILD COMMAND ${TOOL_WINDEPLOYQT} $<TARGET_FILE:texmacs> COMMENT "Running windeployqt...")


set(QTTEXMACS 1)

#find_package(PNG REQUIRED)
#target_link_libraries(texmacs PUBLIC ${PNG_LIBRARIES})

#find_package(Iconv REQUIRED)
#target_link_libraries(texmacs PUBLIC ${ICONV_LIBRARIES})

#find_package(ZLIB REQUIRED)
#target_link_libraries(texmacs PUBLIC ${ZLIB_LIBRARIES})

#find_package(JPEG REQUIRED)
#target_link_libraries(texmacs PUBLIC ${JPEG_LIBRARIES})

file(GLOB_RECURSE TeXmacs_Qt_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Qt/*.cpp")
file(GLOB_RECURSE TeXmacs_Qt_HDRS "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Qt/*.hpp")
target_sources(texmacs PUBLIC ${TeXmacs_Qt_SRCS} ${TeXmacs_Qt_HDRS})


option(QTPIPES "use Qt pipes" ON)

option(USE_SQLITE3 "Use SQLite3" OFF)
if (USE_SQLITE3)
    find_package(SQLite3 REQUIRED)
    option(LINKED_SQLITE3 "Use Linked SQLite3" ON)
    target_link_libraries(texmacs PUBLIC SQLite::SQLite3)
endif (USE_SQLITE3)

add_subdirectory(thirdparty/freetype)
option(LINKED_FREETYPE "linked Freetype" ON)
set(USE_FREETYPE 1)
target_link_libraries(texmacs PUBLIC freetype)
target_include_directories(texmacs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/freetype/include")
file(GLOB_RECURSE TeXmacs_Freetype_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Freetype/*.cpp")
target_sources(texmacs PUBLIC ${TeXmacs_Freetype_SRCS})


option(USE_CAIRO "use Cairo" OFF)
if (USE_CAIRO)
    find_package(Cairo REQUIRED)
    option(LINKED_CAIRO "linked Cairo" ON)
    target_link_libraries(texmacs PUBLIC Cairo::Cairo)
endif (USE_CAIRO)

option(USE_IMLIB2 "use Imlib2" OFF)
if (USE_IMLIB2)
    find_package(Imlib2 REQUIRED)
    option(LINKED_IMLIB2 "linked Imlib2" ON)
    target_link_libraries(texmacs PUBLIC Imlib2::Imlib2)
endif (USE_IMLIB2)

option(USE_SPARKLE "use Sparkle" OFF)
option(PDFHUMMUS_NO_TIFF "Disable TIFF in PDF Hummus" ON)

option(USE_GMP "use GMP" OFF)
if (USE_GMP)
    find_package(GMP)
    option(LINKED_GMP "linked GMP" ON)
    target_link_libraries(texmacs PUBLIC GMP::GMP)
endif (USE_GMP)

set(SCHEME_DECL "")
set(SCHEME_REGI "")

option(ENABLE_INTERNAL_S7 "Use internal s7" ON)
if (ENABLE_INTERNAL_S7)
    message(STATUS "Using internal s7")
    add_subdirectory(thirdparty/s7)
    set(S7_LIBRARIES s7)
    set(USE_S7 ON)
    set(SCHEME_DECL "${SCHEME_DECL} void registerS7();")
    set(SCHEME_REGI "${SCHEME_REGI} registerS7();")
    target_sources(texmacs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Scheme/s7/s7_tmscheme.cpp)
    target_link_libraries(texmacs PUBLIC s7)
else ()
    set(USE_S7 OFF)
endif ()

option(ENABLE_INTERNAL_GUILE18 "Enable internal Guile" ON)
if (ENABLE_INTERNAL_GUILE18)
    message(STATUS "Using internal Guile 1.8.8")
    add_subdirectory(thirdparty/guile)
    set(GUILE_LIBRARIES guile)
    set(USE_GUILE18 ON)
    set(SCHEME_DECL "${SCHEME_DECL} void registerGuile18();")
    set(SCHEME_REGI "${SCHEME_REGI} registerGuile18();")
    target_sources(texmacs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Scheme/Guile18/guile18_scheme.cpp")
    target_link_libraries(texmacs PUBLIC guile)
else()
    set(USE_GUILE18 OFF)
endif()

### --------------------------------------------------------------------
### Include standard modules
### --------------------------------------------------------------------

include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckIncludeFile)
include(CheckTypeSize)
include(FindPkgConfig)
include(CheckCSourceCompiles)
include(FindX11)


### --------------------------------------------------------------------
### Check for standard functionalities
### --------------------------------------------------------------------
check_type_size(FILE HAVE_FILE)
check_include_file(memory.h HAVE_MEMORY_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(X11/Xlib.h HAVE_X11_XLIB_H)
check_include_file(X11/Xutil.h HAVE_X11_XUTIL_H)
check_include_file(pty.h HAVE_PTY_H)
check_include_file(util.h HAVE_UTIL_H)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)


### --------------------------------------------------------------------
### Check for dl library
### --------------------------------------------------------------------

check_function_exists(dlopen HAVE_DLOPEN)

if (NOT ${HAVE_DLOPEN})
    check_library_exists(dl dlopen "" HAVE_DLOPEN)
    if (${HAVE_DLOPEN})
        set(CONFIG_BDL "-ldl")
    endif (${HAVE_DLOPEN})
endif (NOT ${HAVE_DLOPEN})

if (NOT ${HAVE_DLOPEN})
    check_library_exists(dld dld_link "" HAVE_DLOPEN)
    if (${HAVE_DLOPEN})
        set(CONFIG_BDL "-ldl")
    endif (${HAVE_DLOPEN})
endif (NOT ${HAVE_DLOPEN})

if (NOT ${HAVE_DLOPEN})
    check_function_exists(shl_load HAVE_DLOPEN)
    if (${HAVE_DLOPEN})
        set(CONFIG_BDL "-ldl")
    endif (${HAVE_DLOPEN})
endif (NOT ${HAVE_DLOPEN})


if (${HAVE_DLOPEN})
    #SET(TM_DYNAMIC_LINKING 1) # dynamic linking works
endif (${HAVE_DLOPEN})

### --------------------------------------------------------------------
### Handle different systems case by case
### --------------------------------------------------------------------

set(CONFIG_OS "GNU_LINUX")
set(CONFIG_OS_SUFFIX "gnu-linux")
set(CONFIG_CXXTEMPLATE "")
set(CONFIG_STD_SETENV "#define STD_SETENV")
set(CONFIG_SO "so")
set(CONFIG_LIB_PATH "LD_LIBRARY_PATH")
set(CONFIG_CHMOD "chmod -f")
set(CONFIG_CXXOPTIMIZE "-O2")
set(CONFIG_BSTATIC "-Wl,-Bstatic")
set(CONFIG_BSHARED "-Wl,-Bdynamic")
set(CONFIG_BFLAGS "")
set(CONFIG_BPATH "-Wl,-rpath,")
set(CONFIG_WORD_LENGTH "4")
set(CONFIG_WORD_LENGTH_INC "3")
set(CONFIG_WORD_MASK "0xfffffffc")
set(CONFIG_MAX_FAST "260 // WORD_LENGTH more than power of 2")

check_type_size("void*" SIZE_OF_VOIDP)

message(STATUS "Check if we are on a 64-bits computer")
if (${SIZE_OF_VOIDP} EQUAL 8)
    set(CONFIG_WORD_LENGTH "8")
    set(CONFIG_WORD_LENGTH_INC "7")
    set(CONFIG_WORD_MASK "0xfffffffffffffff8")
    set(CONFIG_MAX_FAST "264 // WORD_LENGTH more than power of 2")
endif (${SIZE_OF_VOIDP} EQUAL 8)


if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message(STATUS "final adjustments for an Intel or AMD GNU/Linux host")
    set(CONFIG_CXXOPTIMIZE "-O3 -fexpensive-optimizations")
endif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    message(STATUS "final adjustments for an Intel or AMD GNU/BSD host")
    set(CONFIG_CXXOPTIMIZE "-O3 -fexpensive-optimizations")
endif (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")

if (${CMAKE_SYSTEM_NAME} STREQUAL "SunOS")
    message(STATUS "final adjustments for an Intel or AMS Solaris host")
    set(CONFIG_CXXOPTIMIZE "-O3 -fexpensive-optimizations")
    set(CONFIG_CXXOPTIMIZE "-O3")
    set(CONFIG_BPATH "-Wl,-R,")
    #  SET(X11_LDFLAGS "${X_LIBS} -lXext -lX11 -lsocket")
endif (${CMAKE_SYSTEM_NAME} STREQUAL "SunOS")

if (${CMAKE_SYSTEM_NAME} STREQUAL "CYGWIN_NT-5.1")
    message(STATUS "final adjustments for cygwin host")
    set(CONFIG_OS "CYGWIN")
    set(CONFIG_BFLAGS "-Wl,-stack,8388608")
endif (${CMAKE_SYSTEM_NAME} STREQUAL "CYGWIN_NT-5.1")

### --------------------------------------------------------------------
### Package Data
### --------------------------------------------------------------------

set(PACKAGE_BUGREPORT 1)
set(PACKAGE_NAME 1)
set(PACKAGE_STRING 1)
set(PACKAGE_TARNAME 1)
set(PACKAGE_URL 1)
set(PACKAGE_VERSION 1)

### --------------------------------------------------------------------
### Memory allocation options
### --------------------------------------------------------------------

#if(${DISABLE_FASTALLOC})
#  set(NO_FAST_ALLOC 1)
#endif(${DISABLE_FASTALLOC})


### --------------------------------------------------------------------
### Experimental options
### --------------------------------------------------------------------

#  SET(EXPERIMENTAL 1)

### --------------------------------------------------------------------
###  Test for Libraries
### --------------------------------------------------------------------





### --------------------------------------------------------------------
###  Some Guile related adjustements
### --------------------------------------------------------------------


### --------------------------------------------------------------------
### Generate configure headers
### --------------------------------------------------------------------
set(DEBUG_ASSERT 1)

# set(USE_GS 1)
if (WIN32)
    set(GS_EXE bin/gs.exe)
else (WIN32)
    set(GS_EXE /usr/bin/gs)
endif (WIN32)
set(GS_FONTS ../share/ghostscript/fonts:/usr/share/fonts:)
set(GS_LIB ../share/ghostscript/9.06/lib:)

set(GUILE_NUM 1)
set(PDF_RENDERER 0)
set(SIZEOF_VOID_P 8)
set(STDC_HEADERS 1)

set(CONFIG_STD_SETENV "#define STD_SETENV")
set(tm_orig ${CMAKE_CURRENT_SOURCE_DIR})
set(CONFIG_HOST_OS "linux-gnu") # TODO: should not be hard-coded
set(CONFIG_HOST_VENDOR "pc") # TODO: should not be hard-coded
set(CONFIG_HOST_CPU ${CMAKE_SYSTEM_PROCESSOR})
set(CONFIG_USER $ENV{USER})
string(TIMESTAMP CONFIG_DATE)
set(tm_devel TeXmacs-${DEVEL_VERSION})
set(tm_stable TeXmacs-${STABLE_VERSION})
set(tm_devel_release ${tm_devel}-${DEVEL_RELEASE})
set(tm_stable_release ${tm_stable}-${STABLE_RELEASE})

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${prefix})
set(datarootdir ${prefix}/share)
set(datadir ${datarootdir})
set(tmdata ${datarootdir}/TeXmacs)
set(tmbin ${exec_prefix}/lib/texmacs/TeXmacs)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/System/tm_config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/System/tm_config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/System/tm_configure.hpp.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/System/tm_configure.hpp)
target_include_directories(texmacs PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/src/System)


### --------------------------------------------------------------------
### the include dirs
### --------------------------------------------------------------------
target_include_directories(texmacs PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Data/Convert
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Data/Document
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Data/Drd
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Data/History
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Data/Observers
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Data/Parser
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Data/String
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Data/Tree
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Edit
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Edit/Editor
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Edit/Interface
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Edit/Modify
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Edit/Process
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Edit/Replace
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/Bitmap_fonts
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/Colors
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/Fonts
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/Gui
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/Handwriting
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/Mathematics
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/Pictures
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/Renderer
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/Spacial
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/Types
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Kernel/Abstractions
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Kernel/Containers
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Kernel/Types
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins
       # ${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Pdf/LibAesgm
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Scheme
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Scheme/Scheme
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Style/Environment
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Style/Evaluate
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Style/Memorizer
        ${CMAKE_CURRENT_SOURCE_DIR}/src/System
        ${CMAKE_CURRENT_SOURCE_DIR}/src/System/Boot
        ${CMAKE_CURRENT_SOURCE_DIR}/src/System/Classes
        ${CMAKE_CURRENT_SOURCE_DIR}/src/System/Files
        ${CMAKE_CURRENT_SOURCE_DIR}/src/System/Language
        ${CMAKE_CURRENT_SOURCE_DIR}/src/System/Link
        ${CMAKE_CURRENT_SOURCE_DIR}/src/System/Misc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Texmacs
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Texmacs/Data
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Typeset
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Typeset/Bridge
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Typeset/Concat
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Typeset/Page
        ${CMAKE_CURRENT_SOURCE_DIR}/TeXmacs/include
        ${TEXMACS_BINARY_DIR}/src/System/
        )

if (WIN32 OR MINGW)
    message(STATUS "Using Windows plugins")
    target_include_directories(texmacs PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Windows
            ${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Windows/nowide
            )
    file(GLOB_RECURSE texmacs_windows_plugin_srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Windows/*.cpp")
    target_sources(texmacs PUBLIC ${texmacs_windows_plugin_srcs})
else()
    message(STATUS "Using Unix plugins")
    file(GLOB_RECURSE texmacs_unix_plugin_srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Unix/*.cpp")
    target_sources(texmacs PUBLIC ${texmacs_unix_plugin_srcs})
    target_include_directories(texmacs PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Unix
            )
endif ()

target_include_directories(texmacs PUBLIC
        ${FREETYPE_INCLUDE_DIRS} ${Cairo_INCLUDE_DIRS}
        ${IMLIB2_INCLUDE_DIR} ${GMP_INCLUDES}
        )

### --------------------------------------------------------------------
### the main sources
### --------------------------------------------------------------------
file(GLOB_RECURSE TeXmacs_Base_SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Data/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Edit/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Kernel/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/System/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Texmacs/Data/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Texmacs/Server/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Texmacs/Window/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Typeset/*.cpp"
        )
target_sources(texmacs PUBLIC ${TeXmacs_Base_SRCS})
target_sources(texmacs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Scheme/abstract_scheme.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/Scheme/scheme.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/Scheme/Scheme/glue.cpp")

file(GLOB_RECURSE TeXmacs_Std_Plugins_SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Axel/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Bibtex/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Cairo/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Database/*.cpp"
       # "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Pdf/*.cpp"
       # "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Pdf/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Ghostscript/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Imlib2/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Ispell/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Metafont/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/LaTeX_Preview/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Mplayer/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Openssl/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Sqlite3/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins/Updater/*.cpp"
        )
target_sources(texmacs PUBLIC ${TeXmacs_Std_Plugins_SRCS})

file(GLOB_RECURSE TeXmacs_Style_SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Style/*.cpp"
        )




### --------------------------------------------------------------------
### Determine TeXmacs_Libraries
### --------------------------------------------------------------------

target_link_libraries(texmacs PUBLIC
        ${ZLIB_LIBRARIES}
        ${JPEG_LIBRARIES}
        ${PNG_LIBRARIES}
        )

if (LINKED_CAIRO)
    target_link_libraries(texmacs PUBLIC ${CAIRO_LIBRARIES})
endif (LINKED_CAIRO)



