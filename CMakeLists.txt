cmake_minimum_required(VERSION 3.23)
project(HMM_GestureRecognition)

set(CMAKE_CXX_STANDARD 14)

find_package(PythonLibs COMPONENTS Development 3.10 REQUIRED)
include_directories(. lib)
include_directories(${PYTHON_INCLUDE_DIRS})
#target_link_libraries(HMM_GestureRecognition ${PYTHON_LIBRARIES})

add_executable(HMM_GestureRecognition
        src/HMM.cpp
        src/HMM.h src/HMMcomponents/hiddenState.h src/HMMcomponents/hiddenState.cpp
        src/Gesture.cpp src/Gesture.h src/GestureLibrary.cpp src/GestureLibrary.h utils/externManager.cpp utils/externManager.h)

