cmake_minimum_required(VERSION 3.16)

project(tcpserver VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
# find_package(Boost REQUIRED)
qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(apptcpserver
    main.cpp
    tcpserver.cpp
    tcpserver.h
)

qt_add_qml_module(apptcpserver
    URI tcpserver
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES parameter.h parameter.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(apptcpserver PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.apptcpserver
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(apptcpserver
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS apptcpserver
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
