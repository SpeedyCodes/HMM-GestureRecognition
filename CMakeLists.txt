cmake_minimum_required(VERSION 3.23)
project(HMM_GestureRecognition)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(Python REQUIRED)
find_package(Python REQUIRED COMPONENTS Development)

#include_directories(C:/Users/dasha/AppData/Local/Programs/Python/Python310/include)
include_directories(. lib src/GUI)
include_directories(${Python_INCLUDE_DIRS})
#link_libraries(C:/Users/dasha/AppData/Local/Programs/Python/Python310/libs/python310.lib)

find_package(Qt5 REQUIRED COMPONENTS Widgets Gui Network Multimedia MultimediaWidgets)
#qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(HMM_GestureRecognition
        src/main.cpp
        src/HMM.cpp
        src/HMM.h src/HMMcomponents/hiddenState.h src/HMMcomponents/hiddenState.cpp
        src/Gesture.cpp src/Gesture.h src/GestureLibrary.cpp src/GestureLibrary.h utils/externManager.cpp utils/externManager.h
        src/GUI/mainwindow.cpp src/GUI/mainwindow.h src/GUI/mainwindow.ui
        src/GUI/GestureEditor.h src/GUI/GestureEditor.ui src/GUI/GestureEditor.cpp
        src/GUI/SignLanguageWriter.h src/GUI/SignLanguageWriter.ui src/GUI/SignLanguageWriter.cpp
        src/GUI/RobotConnectionManager.h src/GUI/RobotConnectionManager.cpp src/GUI/RobotConnectionManager.ui
        src/MediapipeInterface/MediapipeInterface.cpp src/MediapipeInterface/MediapipeInterface.h tests/deleteThisTest.cpp
        )

target_link_libraries(HMM_GestureRecognition PRIVATE Qt5::Widgets Qt5::Gui Qt5::Network Qt5::Multimedia Qt5::MultimediaWidgets
        ${Python_LIBRARIES})
set_target_properties(HMM_GestureRecognition PROPERTIES
        #WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
        )

