file(GLOB_RECURSE TeXmacs_Std_Plugins_SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/Axel/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Bibtex/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Cairo/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Database/*.cpp"
        # "${CMAKE_CURRENT_SOURCE_DIR}/Pdf/*.cpp"
        # "${CMAKE_CURRENT_SOURCE_DIR}/Pdf/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/Ghostscript/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Imlib2/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Ispell/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Metafont/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/LaTeX_Preview/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Mplayer/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Openssl/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Sqlite3/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Updater/*.cpp"
        )
target_sources(texmacs-gui PUBLIC ${TeXmacs_Std_Plugins_SRCS})

file(GLOB_RECURSE TeXmacs_Qt_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/Qt/*.cpp")
file(GLOB_RECURSE TeXmacs_Qt_HDRS "${CMAKE_CURRENT_SOURCE_DIR}/Qt/*.hpp")
target_sources(texmacs-gui PUBLIC ${TeXmacs_Qt_SRCS} ${TeXmacs_Qt_HDRS})
option(QTPIPES "use Qt pipes" ON)


target_include_directories(texmacs-gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


if (WIN32 OR MINGW)
    message(STATUS "Using Windows plugins")
    target_include_directories(texmacs-gui PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Windows
            ${CMAKE_CURRENT_SOURCE_DIR}/Windows/nowide
            )
    file(GLOB_RECURSE texmacs_windows_plugin_srcs "${CMAKE_CURRENT_SOURCE_DIR}/Windows/*.cpp")
    target_sources(texmacs-gui PUBLIC ${texmacs_windows_plugin_srcs})
else()
    message(STATUS "Using Unix plugins")
    file(GLOB_RECURSE texmacs_unix_plugin_srcs "${CMAKE_CURRENT_SOURCE_DIR}/Unix/*.cpp")
    target_sources(texmacsgui PUBLIC ${texmacs_unix_plugin_srcs})
    target_include_directories(texmacs-gui PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Unix
            )
endif ()

if (USE_FREETYPE)
    file(GLOB_RECURSE TeXmacs_Freetype_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/Freetype/*.cpp")
    target_sources(texmacs-gui PUBLIC ${TeXmacs_Freetype_SRCS})
    target_link_libraries(texmacs-gui PUBLIC ${FREETYPE_LIBRARIES})
    target_include_directories(texmacs-gui PUBLIC ${FREETYPE_INCLUDE_DIRS})
endif ()