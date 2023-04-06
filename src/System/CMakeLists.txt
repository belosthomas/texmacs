file(GLOB_RECURSE TEXMACS_SYSTEM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE TEXMACS_SYSTEM_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
target_sources(texmacs-gui PRIVATE ${TEXMACS_SYSTEM_SOURCES} ${TEXMACS_SYSTEM_HEADERS})

target_include_directories(texmacs-gui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Boot
        ${CMAKE_CURRENT_SOURCE_DIR}/Classes
        ${CMAKE_CURRENT_SOURCE_DIR}/Files
        ${CMAKE_CURRENT_SOURCE_DIR}/Language
        ${CMAKE_CURRENT_SOURCE_DIR}/Link
        ${CMAKE_CURRENT_SOURCE_DIR}/Misc
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tm_config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/tm_config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tm_configure.hpp.cmake ${CMAKE_CURRENT_BINARY_DIR}/tm_configure.hpp)
target_include_directories(texmacs-gui PUBLIC ${CMAKE_CURRENT_BINARY_DIR})