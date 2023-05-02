cmake_minimum_required(VERSION 3.24)
project(HMM_GestureRecognition)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(HMM_GestureRecognition
        src/HMM.cpp
        src/HMM.h src/HMMcomponents/hiddenState.h src/HMMcomponents/hiddenState.cpp src/HMMcomponents/Observable.h)