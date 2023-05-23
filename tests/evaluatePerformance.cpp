//
// Created by dasha on 22.05.2023.
//
#include "src/GestureLibrary.h"
#include "src/utils/MediapipeInterface.h"
#include <iostream>
#include <filesystem>


void evaluateRecognizeGesture(GestureLibrary& library, std::string pathFolderToEvaluate){
    MediapipeInterface* minterface = new MediapipeInterface;
    unsigned int allGoods = 0;
    unsigned int allTries = 0;
    for(std::pair<std::string, Gesture> gestureTuple:library.getGestures()){
        std::string gestureName = gestureTuple.first;
        Gesture gesture = gestureTuple.second;
        std::string gestureFolder = pathFolderToEvaluate + "/" + gestureName;
        unsigned int gestureSucces = 0;
        unsigned int gestureAll = 0;
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
    std::cout << "Congratulation, it's " << allGoods*100.0/allTries << "%" << std::endl;
    delete minterface;
}
void alt_evaluateRecognizeGesture(GestureLibrary& library, const std::vector<std::string>& videosToEvaluate){
    MediapipeInterface* minterface = new MediapipeInterface;
    unsigned int allGoods = 0;
    unsigned int allTries = 0;
    for(std::pair<std::string, Gesture> gestureTuple:library.getGestures()){
        std::string gestureName = gestureTuple.first;
        Gesture gesture = gestureTuple.second;
        unsigned int gestureSucces = 0;
        unsigned int gestureAll = 0;
        for(const std::string& videoPath:videosToEvaluate){
            if(videoPath.find(gestureName) != std::string::npos){
                std::string predictedGesture = library.recognizeFromVideo(videoPath.c_str(), minterface);
                if(predictedGesture == gestureName) gestureSucces++;
                gestureAll++;
            }
        }
        std::cerr << "Result for " << gestureName << " = " << gestureSucces << "/" << gestureAll << std::endl;
        allGoods += gestureSucces;
        allTries += gestureAll;
    }
    std::cerr << "Result for the whole library is " << allGoods << "/" << allTries << std::endl;
    std::cout << "Congratulation, it's " << allGoods*100.0/allTries << "%" << std::endl;
    delete minterface;
}

int main(){
    std::string libPath = "tests/182/182.gesturelibrary";
    GestureLibrary library(libPath);
    std::string path = R"(C:\Users\dasha\PycharmProjects\HMM-GestureRecognition-Collect-data\HMM-GestureRecognition-Collect-data\Collected_videos)";
    evaluateRecognizeGesture(library, "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/");

//    alt_evaluateRecognizeGesture(library, {"C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/eight/eight0_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/eight/eight1_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/eight/eight2_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/eight/eight3_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/eight/eight4_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/eight/eight5_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/eight/eight6_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/eight/eight7_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/eight/eight8_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/eight/eight9_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/two/two0_0.avi",
//            "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/two/two1_0.avi",
//            "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/two/two2_0.avi",
//            "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/two/two3_0.avi",
//            "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/two/two4_0.avi",
//            "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/two/two5_0.avi",
//            "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/two/two6_0.avi",
//            "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/two/two7_0.avi",
//            "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/two/two8_0.avi",
//            "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/two/two9_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/one/one0_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/one/one1_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/one/one2_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/one/one3_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/one/one4_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/one/one5_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/one/one6_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/one/one7_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/one/one8_0.avi",
//                                           "C:/Users/dasha/PycharmProjects/HMM-GestureRecognition-Collect-data/HMM-GestureRecognition-Collect-data/Collected_videos/one/one9_0.avi"
//                                 }
//    );
    return 0;
}
