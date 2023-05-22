//
// Created by dasha on 22.05.2023.
//
#include "src/GestureLibrary.h"
#include "src/utils/MediapipeInterface.h"
#include <iostream>
#include <filesystem>
#include <QDir>
#include <windows.h>


void evaluateRecognizeGesture(GestureLibrary& library, std::string pathFolderToEvaluate){
    MediapipeInterface* minterface;
    unsigned int allGoods = 0;
    unsigned int allTries = 0;
    for(std::pair<std::string, Gesture> gestureTuple:library.getGestures()){
        std::string gestureName = gestureTuple.first;
        Gesture gesture = gestureTuple.second;
        const char* gestureFolder = (pathFolderToEvaluate + "/" + gestureName).c_str();
        unsigned int gestureSucces = 0;
        unsigned int gestureAll = 0;
//        WIN32_FIND_DATAA fileData;
//        HANDLE hFind = FindFirstFileA(gestureFolder, &fileData);
//
//        if (hFind != INVALID_HANDLE_VALUE) {
//            do {
//                if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
//                    std::cout << fileData.cFileName << std::endl;
//                    // Perform operations on the file here
//                    const char* path = reinterpret_cast<const char *>(fileData.cFileName);
//                    std::string predictedGesture = library.recognizeFromVideo(path, minterface);
//                    if(predictedGesture == gestureName) gestureSucces++;
//                    gestureAll++;
//                }
//            } while (FindNextFileA(hFind, &fileData));
//
//            FindClose(hFind);
//        }
        for (const auto& entry : filesystem::directory_iterator(gestureFolder)) {
            if (entry.is_regular_file()) {
                std::cout << entry.path() << std::endl;
                const char* path = reinterpret_cast<const char *>(entry.path().c_str());
                std::string predictedGesture = library.recognizeFromVideo(path, minterface);
                if(predictedGesture == gestureName) gestureSucces++;
                gestureAll++;
            }
        }
        std::cerr << "Result for " << gestureName << " = " << gestureSucces << "/" << gestureAll << std::endl;
        allGoods += gestureSucces;
        allTries += gestureAll;
    }
    std::cerr << "Result for the whole library is " << allGoods << "/" << allTries << std::endl;
    std::cout << "Congratulation, it's " << allGoods*100/allTries << "%" << std::endl;
}

int main(){
    GestureLibrary library((string &) "tests/getallen/getallen.gesturelibrary");
    // Set pathtoFolder
    std::string path = R"(C:\Users\dasha\PycharmProjects\HMM-GestureRecognition-Collect-data\HMM-GestureRecognition-Collect-data\Collected_videos)";
    // start evaluateRecognizeGesture
    evaluateRecognizeGesture(library, path);
    return 0;
}