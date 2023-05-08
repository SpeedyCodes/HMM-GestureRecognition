//
// Created by dasha on 08.05.2023.
//

#ifndef HMM_GESTURERECOGNITION_GESTURELIBRARY_H
#define HMM_GESTURERECOGNITION_GESTURELIBRARY_H
#include <iostream>
#include <vector>
#include <map>

typedef int Observable;
class Gesture;
class HMM;

class GestureLibrary {
private:
    std::map<std::string, Gesture> gestures;
    std::vector<Observable > accumulatedLiveFeedData;
    HMM* getThresholdHMM() const;
public:
    GestureLibrary();
    void fromDirectory(std::string& directory);
    void toDirectory(std::string& directory) const;
    bool modelTrainingAndSelection(std::vector<std::vector<Observable>>& observed, std::string gestureID);
    std::string recognizeGesture(std::vector<Observable>& observed) const;
    bool isolatedRecognition(std::string& videoPath, std::string& gestureID);
    bool realtimeRecognition(std::string& gestureID);
};


#endif //HMM_GESTURERECOGNITION_GESTURELIBRARY_H
