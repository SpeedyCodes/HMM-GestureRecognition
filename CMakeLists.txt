cmake_minimum_required(VERSION 3.17)
project(HMM_GestureRecognition)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python REQUIRED COMPONENTS Development)

include_directories(. lib src/GUI)
include_directories(${Python_INCLUDE_DIRS})
# You may specify the path to Python lib manually
link_libraries(C:/Users/dasha/AppData/Local/Programs/Python/Python310/libs/python310.lib)
# Find Qt5 or Qt6
find_package(Qt5 REQUIRED COMPONENTS Widgets Network Multimedia MultimediaWidgets)

if (QT_VERSION_MAJOR EQUAL 6)
    # Qt6 is found
    qt_standard_project_setup()
else()
    # Qt5 is found
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
endif()


add_executable(HMM_GestureRecognition
        src/main.cpp
        src/HMM.cpp
        src/HMM.h src/HMMcomponents/hiddenState.h src/HMMcomponents/hiddenState.cpp
        src/Gesture.cpp src/Gesture.h src/GestureLibrary.cpp src/GestureLibrary.h
        src/GUI/mainwindow.cpp src/GUI/mainwindow.h src/GUI/mainwindow.ui
        src/GUI/GestureEditor.h src/GUI/GestureEditor.ui src/GUI/GestureEditor.cpp
        src/GUI/SignLanguageWriter.h src/GUI/SignLanguageWriter.ui src/GUI/SignLanguageWriter.cpp
        src/GUI/RobotConnectionManager.h src/GUI/RobotConnectionManager.cpp src/GUI/RobotConnectionManager.ui
        src/utils/MediapipeInterface.cpp src/utils/MediapipeInterface.h
        )

target_link_libraries(HMM_GestureRecognition PRIVATE Qt5::Widgets Qt5::Network Qt5::Multimedia Qt5::MultimediaWidgets)
set_target_properties(HMM_GestureRecognition PROPERTIES
        #WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
        )