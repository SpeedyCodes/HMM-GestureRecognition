cmake_minimum_required(VERSION 3.16)
project(HMM_GestureRecognition)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python REQUIRED COMPONENTS Development)

include_directories(. lib src/GUI)
# TO BE EDITED BY USER
add_compile_definitions(PYTHON_COMMAND="python3") # the command to run python, may need to be changed to just "python"
# Python headers
# example: include_directories(/usr/include/python3.10)
# Python binaries
# Linux example: link_libraries(/usr/lib/python3.10/config-3.10-x86_64-linux-gnu/libpython3.10.so)
# Windows example: link_libraries(C:/Users/dasha/AppData/Local/Programs/Python/Python310/libs/python310.lib)

# Find Qt5 or Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Network Multimedia MultimediaWidgets)

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
        src/utils/MediapipeInterface.cpp src/utils/MediapipeInterface.h src/applications/Robot/robotcontroller.cpp src/HMMcomponents/logProbability.cpp src/HMMcomponents/logProbability.h)

add_executable(Tests tests/tests.cpp src/HMM.cpp src/HMM.h src/HMMcomponents/hiddenState.h
        src/HMMcomponents/hiddenState.cpp src/Gesture.cpp src/Gesture.h src/HMMcomponents/logProbability.cpp src/HMMcomponents/logProbability.h)

add_executable(Performance tests/evaluatePerformance.cpp src/utils/MediapipeInterface.cpp src/utils/MediapipeInterface.h
        src/HMM.cpp
        src/HMM.h src/HMMcomponents/hiddenState.h src/HMMcomponents/hiddenState.cpp
        src/Gesture.cpp src/Gesture.h src/GestureLibrary.cpp src/GestureLibrary.h src/HMMcomponents/logProbability.cpp src/HMMcomponents/logProbability.h)

set(Qt_Libs Qt6::Widgets Qt6::Network Qt6::Multimedia Qt6::MultimediaWidgets)
target_link_libraries(HMM_GestureRecognition PRIVATE ${Qt_Libs})
target_link_libraries(Performance PRIVATE ${Qt_Libs})
set_target_properties(HMM_GestureRecognition PROPERTIES
        #WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
        )
