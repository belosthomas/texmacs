# A function to generate the resources.qrc file
function(GenerateTeXMacsResourcesQRC)

    # Generate resources.qrc
    file(GLOB_RECURSE Texmacs_Resources "${TEXMACS_RESOURCES_DIR}/*")

    set(Texmacs_Resources_List "")

    # For each file
    foreach (Texmacs_Resource ${Texmacs_Resources})
        # Get the file name
        get_filename_component(Texmacs_Resource_Name ${Texmacs_Resource} NAME)
        # Get the file path
        get_filename_component(Texmacs_Resource_Path ${Texmacs_Resource} PATH)
        # Remove the path prefix
        string(REPLACE "${TEXMACS_RESOURCES_DIR}" "" Texmacs_Resource_Path ${Texmacs_Resource_Path})
        # Add the file to the resources
        # list(APPEND Texmacs_Resources_List "<file alias=\"${Texmacs_Resource_Path}/${Texmacs_Resource_Name}\">${Texmacs_Resource}</file>")
        set(Texmacs_Resources_List "${Texmacs_Resources_List}<file alias=\"TeXmacs/${Texmacs_Resource_Path}/${Texmacs_Resource_Name}\">${Texmacs_Resource}</file>\n")
    endforeach ()

    # Generate the qrc file
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc.in" "${CMAKE_CURRENT_BINARY_DIR}/tm_resources.qrc" @ONLY)

    if (CMAKE_AUTORCC)

        # Add the qrc to the texmacs target
        target_sources(texmacs-gui PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/tm_resources.qrc")

    else ()

        # Add the qrc to the texmacs target
        qt_add_resources(TEXMACS_QRC "${CMAKE_CURRENT_BINARY_DIR}/tm_resources.qrc")

        # add the qrc to the texmacs target
        target_sources(texmacs-gui PUBLIC "${TEXMACS_QRC}")

    endif ()

endfunction()


# Create the executable using Qt commands
qt_add_executable(texmacs-gui

        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Application.hpp

        ${CMAKE_CURRENT_SOURCE_DIR}/DocumentWidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DocumentWidget.hpp

        ${CMAKE_CURRENT_SOURCE_DIR}/DrawBoard.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DrawBoard.hpp

        ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.hpp

        ${CMAKE_CURRENT_SOURCE_DIR}/PixmapManager.hpp

        ${CMAKE_CURRENT_SOURCE_DIR}/ResourcesExtractor.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ResourcesExtractor.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/ThingyTabInnerWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ThingyTabInnerWindow.hpp

        ${CMAKE_CURRENT_SOURCE_DIR}/WebBrowser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/WebBrowser.hpp

        ${CMAKE_CURRENT_SOURCE_DIR}/WelcomeWidget.hpp
        )

if (NOT CMAKE_AUTOMOC)

    # Run Qt Moc
    qt_generate_moc(
            ${CMAKE_CURRENT_SOURCE_DIR}/Application.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/Application.moc
    )
    qt_generate_moc(
            ${CMAKE_CURRENT_SOURCE_DIR}/DocumentWidget.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/DocumentWidget.moc
    )
    qt_generate_moc(
            ${CMAKE_CURRENT_SOURCE_DIR}/DrawBoard.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/DrawBoard.moc
    )
    qt_generate_moc(
            ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/MainWindow.moc
    )
    qt_generate_moc(
            ${CMAKE_CURRENT_SOURCE_DIR}/ResourcesExtractor.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/ResourcesExtractor.moc
    )
    qt_generate_moc(
            ${CMAKE_CURRENT_SOURCE_DIR}/ThingyTabInnerWindow.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/ThingyTabInnerWindow.moc
    )
    qt_generate_moc(
            ${CMAKE_CURRENT_SOURCE_DIR}/WebBrowser.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/WebBrowser.moc
    )
    qt_generate_moc(
            ${CMAKE_CURRENT_SOURCE_DIR}/WelcomeWidget.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/WelcomeWidget.moc
    )

    # Add the moc files to the texmacs target
    target_sources(texmacs-gui PUBLIC
            ${CMAKE_CURRENT_BINARY_DIR}/Application.moc
            ${CMAKE_CURRENT_BINARY_DIR}/DocumentWidget.moc
            ${CMAKE_CURRENT_BINARY_DIR}/DrawBoard.moc
            ${CMAKE_CURRENT_BINARY_DIR}/MainWindow.moc
            ${CMAKE_CURRENT_BINARY_DIR}/ResourcesExtractor.moc
            ${CMAKE_CURRENT_BINARY_DIR}/ThingyTabInnerWindow.moc
            ${CMAKE_CURRENT_BINARY_DIR}/WebBrowser.moc
            ${CMAKE_CURRENT_BINARY_DIR}/WelcomeWidget.moc
    )

endif ()


set_target_properties(texmacs-gui PROPERTIES QT_ANDROID_PACKAGE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/android")

# Generate the executable on the root directory
# set_target_properties(texmacs-gui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Change the executable name
# set_target_properties(texmacs-gui PROPERTIES OUTPUT_NAME "TeXmacs")

# Link the executable to Qt
target_link_libraries(texmacs-gui PUBLIC Qt::Gui Qt::Widgets Qt::PrintSupport Qt::Svg Qt::OpenGL Qt::QuickWidgets)

# Generate the resources.qrc file
GenerateTeXMacsResourcesQRC()

if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set_target_properties(texmacs-gui PROPERTIES MACOSX_BUNDLE ON)
    set_target_properties(texmacs-gui PROPERTIES WIN32_EXECUTABLE ON)
endif ()
