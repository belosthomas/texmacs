# CMakeLists.txt for TeXmacs entirely rewritten by Liza Belos

#######################################################################################################################
#                                                                                                                     #
#  Project Informations                                                                                               #
#                                                                                                                     #
#######################################################################################################################

cmake_minimum_required(VERSION 3.13)
project(TeXmacs VERSION 2.1.2 DESCRIPTION "TeXmacs is a scientific text editor" HOMEPAGE_URL "https://www.texmacs.org" LANGUAGES CXX C)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

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

#######################################################################################################################
#                                                                                                                     #
#  Find Qt                                                                                                            #
#                                                                                                                     #
#######################################################################################################################

option(QT_VERSION "Qt version to use" "6")

if (QT_VERSION STREQUAL "5" OR NOT QT_VERSION)
    message(STATUS "Using Qt5")
    find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport Svg OpenGL QuickWidgets)
    if (NOT Qt5_FOUND)
        message(FATAL_ERROR "Qt5.15 or higher version not found. Please specify the -DQt5_DIR (exemple -DQt5_DIR=D:/Qt/5.15.2/msvc2019_64/lib/cmake) variable or set the -DQT_VERSION option to 6")
    else()
        message(STATUS "Found Qt5 !")
    endif()
endif()

if (QT_VERSION STREQUAL "6")
    message(STATUS "Using Qt6")
    find_package(Qt6 COMPONENTS Core Gui Widgets PrintSupport Svg OpenGL QuickWidgets)
    if (NOT Qt6_FOUND)
        message(FATAL_ERROR "Qt6 not found. Please specify the -DQt6_DIR variable or set the -DQT_VERSION option to 5")
    else()
        message(STATUS "Found Qt6 !")
    endif()
endif()

if (NOT Qt6_FOUND AND NOT Qt5_FOUND)
    message(FATAL_ERROR "Qt not found. Please specify the -DQt5_DIR or -DQt6_DIR variable or set the -DQT_VERSION option to 5 or 6")
endif()

if (Qt6_FOUND)
    set(USE_GUILE_QT6 ON CACHE BOOL "Use Guile with Qt" FORCE)
endif()

if (Qt5_FOUND)
    set(USE_GUILE_QT5 ON CACHE BOOL "Use Guile with Qt" FORCE)
endif()


#######################################################################################################################
#                                                                                                                     #
#  Internal Thirdparty                                                                                                #
#                                                                                                                     #
#######################################################################################################################

# FreeType
add_subdirectory(thirdparty/freetype)
option(LINKED_FREETYPE "linked Freetype" ON)
set(USE_FREETYPE 1)
set(FREETYPE_LIBRARIES freetype)
set(FREETYPE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/freetype/include")

# S7
add_subdirectory(thirdparty/s7)
set(S7_LIBRARIES s7)
set(USE_S7 ON)

# Guile18
message(STATUS "Using internal Guile 1.8.8")
add_subdirectory(thirdparty/guile)
set(GUILE_LIBRARIES guile)
set(USE_GUILE18 ON)


#######################################################################################################################
#                                                                                                                     #
#  External Thirdparty                                                                                                #
#                                                                                                                     #
#######################################################################################################################

# Find SQLite3
option(USE_SQLITE3 "Use SQLite3" OFF)
if (USE_SQLITE3)
    find_package(SQLite3 REQUIRED)
    option(LINKED_SQLITE3 "Use Linked SQLite3" ON)
    target_link_libraries(texmacs PUBLIC SQLite::SQLite3)
endif (USE_SQLITE3)

# Cairo
option(USE_CAIRO "use Cairo" OFF)
if (USE_CAIRO)
    find_package(Cairo REQUIRED)
    option(LINKED_CAIRO "linked Cairo" ON)
    target_link_libraries(texmacs PUBLIC Cairo::Cairo)
endif (USE_CAIRO)

# Imlib2
option(USE_IMLIB2 "use Imlib2" OFF)
if (USE_IMLIB2)
    find_package(Imlib2 REQUIRED)
    option(LINKED_IMLIB2 "linked Imlib2" ON)
    target_link_libraries(texmacs PUBLIC Imlib2::Imlib2)
endif (USE_IMLIB2)

option(USE_SPARKLE "use Sparkle" OFF)
option(PDFHUMMUS_NO_TIFF "Disable TIFF in PDF Hummus" ON)

# GMP
option(USE_GMP "use GMP" OFF)
if (USE_GMP)
    find_package(GMP)
    option(LINKED_GMP "linked GMP" ON)
    target_link_libraries(texmacs PUBLIC GMP::GMP)
endif (USE_GMP)

#######################################################################################################################
#                                                                                                                     #
#  Global Definitions                                                                                                 #
#                                                                                                                     #
#######################################################################################################################

set(TEXMACS_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/TeXMacs")

# Global variables that should be removed
if (MINGW OR WIN32)
     set (OS_MINGW 1)
endif()
set(PACKAGE TeXmacs)

set(PACKAGE_BUGREPORT 1)
set(PACKAGE_NAME 1)
set(PACKAGE_STRING 1)
set(PACKAGE_TARNAME 1)
set(PACKAGE_URL 1)
set(PACKAGE_VERSION 1)

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

#######################################################################################################################
#                                                                                                                     #
#  TeXMacs Gui                                                                                                        #
#                                                                                                                     #
#######################################################################################################################

add_subdirectory(src/Gui)

#######################################################################################################################
#                                                                                                                     #
#  TeXMacs Core Library                                                                                               #
#                                                                                                                     #
#######################################################################################################################

target_include_directories(texmacs-gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(texmacs-gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/TeXmacs/include)
if(WIN32)
    target_link_libraries(texmacs-gui PUBLIC wsock32 ws2_32)
endif()

# Compile options : Build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE MinSizeRel CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(texmacs-gui PUBLIC -O3 -fexpensive-optimizations)
endif()
if (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    target_compile_options(texmacs-gui PUBLIC -Os)
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(texmacs-gui PUBLIC -O0)
endif()

# Compile options : Definitions
target_compile_definitions(texmacs-gui PUBLIC -DWIN32_LEAN_AND_MEAN)
target_compile_definitions(texmacs-gui PUBLIC -DQTTEXMACS=1)

add_subdirectory(src/Data)
add_subdirectory(src/Edit)
add_subdirectory(src/Graphics)
add_subdirectory(src/Kernel)
add_subdirectory(src/Plugins)
add_subdirectory(src/Scheme)
add_subdirectory(src/Style)
add_subdirectory(src/System)
add_subdirectory(src/Texmacs)
add_subdirectory(src/Typeset)
add_subdirectory(src/Utils)
