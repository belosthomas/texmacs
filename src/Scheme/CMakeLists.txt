# These two variables are used to register the scheme in the C++ code
# SCHEME_DECL contains the declaration of the register function, returning factory
# SCHEME_REGI contains the call to the register function, creating all the factories
set(SCHEME_DECL "")
set(SCHEME_REGI "")

# Add Scheme sources
file(GLOB SCHEME_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB SCHEME_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
target_sources(texmacs-gui PUBLIC ${SCHEME_SOURCES} ${SCHEME_HEADERS})

# When using S7
if (USE_S7)
    # Glob all files in the s7 directory
    file(GLOB S7_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/s7/*.cpp")
    file(GLOB S7_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/s7/*.hpp")
    target_sources(texmacs-gui PUBLIC ${S7_SOURCES} ${S7_HEADERS})

    # Link the s7 library
    target_link_libraries(texmacs-gui PUBLIC ${S7_LIBRARIES})

    # Register the s7 scheme in the C++ code
    set(SCHEME_DECL "${SCHEME_DECL} void registerS7();")
    set(SCHEME_REGI "${SCHEME_REGI} registerS7();")
endif()

# When using Guile18
if (USE_GUILE18)
    # Glob all files in the Guile18 directory
    file(GLOB GUILE18_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Guile18/*.cpp")
    file(GLOB GUILE18_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Guile18/*.hpp")
    target_sources(texmacs-gui PUBLIC ${GUILE18_SOURCES} ${GUILE18_HEADERS})

    # Link the Guile18 library
    target_link_libraries(texmacs-gui PUBLIC ${GUILE_LIBRARIES})

    # Register the Guile18 scheme in the C++ code
    set(SCHEME_DECL "${SCHEME_DECL} void registerGuile18();")
    set(SCHEME_REGI "${SCHEME_REGI} registerGuile18();")
endif()

# Configure file scheme_config.cmake.hpp to generate scheme_config.hpp
configure_file(scheme_config.cmake.hpp scheme_config.hpp)
target_include_directories(texmacs-gui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR})